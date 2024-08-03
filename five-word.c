#include "five-word.h"


int main(int argc, char *argv[]){

    if (argc < 3){
        printf("Usage: %s <thread count> <wordlist>\n", argv[0]);
        return 1;
    }

    uint32_t ThreadCount = atoi(argv[1]);
    if (ThreadCount < 1 || ThreadCount > MaxThreads){
        printf("ERR: Thread count must be between 1 and %d\n", MaxThreads);
        return 1;
    }
   
    Pause = 1;

    for (uint32_t i = 0; i < ThreadCount; i++){
        int thr = pthread_create(&(Threads[i]), NULL, &Calc, NULL);
        if (thr){
            printf("ERR: Could not create thread %d", thr);
        }
    }

    if (pthread_mutex_init(&CalcMutex, NULL)){
        printf("ERR: Could not initialise calculation mutex\n");
        return 1;
    }
    if (pthread_mutex_init(&OutputMutex, NULL)){
        printf("ERR: Could not initialise output mutex\n");
        return 1;
    }

    FILE *WordFile = fopen(argv[2], "r");
    if (!WordFile){
        printf("ERR: Could not open word file %s\n", argv[2]);
    }

    AllWords = malloc(sizeof(Word) * MaxWords);
    if (!AllWords){
        printf("ERR: Could not allocate main word array\n");
        return 1;
    }
    
    WordCount = LoadWordFile(WordFile);
    if (WordCount < 5){
        printf("ERR: Word file contains less than 5 valid words.");
        return 1;
    }

    fclose(WordFile);
    
    WordList = malloc(sizeof(Word) * WordCount);
    memcpy(WordList, AllWords, WordCount * sizeof(Word));
    free(AllWords);

    //Test to see if memcpy is working
    /*printf("WordCount: %u\n", WordCount);
    for (int i = 0; i < WordCount; i++){
        printf("%s\n", WordList[i].Chars);
    }*/

    GetIntersects(WordList, WordCount);
        
    Pause = 0;

    for (uint32_t i = 0; i < ThreadCount; i++){
        pthread_join(Threads[i], NULL);
    }
}



uint32_t LoadWordFile(FILE *WordFile){
    uint32_t wordIndex = 0;
    uint32_t validWord = 1;

    while (fgets(AllWords[wordIndex].Chars, MaxWordLength, WordFile)){
        char *currentWord = AllWords[wordIndex].Chars;
        AllWords[wordIndex].LetterMask = 0;
        validWord = 1;

        if (currentWord[5] == '\r'){ //if 5 letter word
            currentWord[5] = 0;
            for (int i = 0; i < 5; i++){
                if (currentWord[i] < 'a' || currentWord[i] > 'z'){
                    printf("ERR: Word file contains invalid chars (Must only contain lowercase a-z)\n");
                    return 1;
                }
                else{
                    if (AllWords[wordIndex].LetterMask & (1U << (currentWord[i] - 'a'))){
                        validWord = 0;
                        break;
                    }
                    else{
                        AllWords[wordIndex].LetterMask |= (1U << (currentWord[i] - 'a'));
                        //printf("Test: %s, %u\n", currentWord, wordIndex);
                    }
                }
            }
            
        }
        else {
            validWord = 0;
        }
        if (validWord){
            //printf("Valid\n");
            uint8_t is_anagram = 0;
            for (int i = 0; i < wordIndex; i++){
                if (AllWords[i].LetterMask == AllWords[wordIndex].LetterMask){
                    is_anagram = 1;
                    //printf("Anagram: %s, %s, %u\n", AllWords[wordIndex].Chars, AllWords[i].Chars, AllWords[i].LetterMask);
                    break;
                }
            }
            if (is_anagram == 0){
                wordIndex++;
                //printf("Increment\n");
            }
        }
    }
    printf("Valid word count: %u\n", wordIndex);

    return wordIndex;
}

void GetIntersects(Word *wordList, uint32_t numWords){
   for (uint32_t i = 0; i < numWords - 2; i++){
       Word *word = &(wordList[i]);
       word->numIntersects = 0;
       for (uint32_t j = i + 1; j < numWords - 1; j++){
           Word *matchWord = &(wordList[j]);

           if ((word->LetterMask & matchWord->LetterMask) == 0){
               word->Intersects[word->numIntersects] = j;
               word->numIntersects++;
           }
       }
   } 
}

void *Calc(void* arg){
    while (Pause){

    }

    while (1){

    }
}
