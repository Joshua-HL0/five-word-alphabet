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

    if (LoadWordFile(WordFile) == 1){
        return 1;
    }
    fclose(WordFile);
        
}



uint8_t LoadWordFile(FILE *WordFile){
    WordCount = 0;
    uint32_t wordIndex = 0;
    uint32_t validWord = 1;

    while (fgets(AllWords[wordIndex].Chars, MaxWordLength, WordFile)){
        char *currentWord = AllWords[wordIndex].Chars;
        validWord = 1;

        if (currentWord[5] == '\r'){ //if 5 letter word
            currentWord[5] = 0;
            for (int i = 0; i < 5; i++){
                if (currentWord[i] < 'a' || currentWord[i] > 'z'){
                    printf("ERR: Word file contains invalid chars (Must only contain lowercase a-z)\n");
                    return 1;
                }
                else{
                    if (AllWords[wordIndex].LetterMask & (1 << (currentWord[i] - 'a'))){
                        validWord = 0;
                        break;
                    }
                    else{
                        AllWords[wordIndex].LetterMask |= (1 << (currentWord[i] - 'a'));
                        printf("Test: %s, %u\n", currentWord, wordIndex);
                    }
                }
            }
            
        }
        if (validWord){
            wordIndex++;
        }
    }
    printf("Valid word count: %u\n", wordIndex);
    return 0;
}


