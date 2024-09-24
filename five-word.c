#include "five-word.h"
#include <stdint.h>
#include <stdio.h>


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
        printf("ERR: Word file contains less than 5 valid words.\n");
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
    
    int num = 93;
    Word *word = &(WordList[num]);

    printf("Test: Word: %s, num: %u, intersects:\n", word->Chars, num);
    for (int i = 0; i < word->Intersects.numIntersects; i++){
        printf("%s,  %u\n", WordList[word->Intersects.IntersectArr[i]].Chars, word->Intersects.IntersectArr[i]);
    }

    Pause = 0;
    printf("Pause is 0\n");

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
    printf("starting getintersects with %u words\n", numWords);
   for (uint32_t i = 0; i < numWords - 2; i++){
       Word *word = &(wordList[i]);
       word->Intersects.numIntersects = 0;
       for (uint32_t j = i + 1; j < numWords - 1; j++){
           Word *matchWord = &(wordList[j]);

           if ((word->LetterMask & matchWord->LetterMask) == 0){
               word->Intersects.IntersectArr[word->Intersects.numIntersects] = j;
               word->Intersects.numIntersects++;
               if (word->Intersects.numIntersects >= MaxIntersects){
                   printf("Warning: Reached MaxIntersects(%d) for word %u\n", MaxIntersects, i);
               }
           }
       }

   }
   printf("Finished getIntersects\n");
}

void *Calc(void* arg){
    uint32_t threadId = (uint32_t)(uintptr_t)arg;
    printf("Thread %u started\n", threadId);

    uint32_t wordIndex;
    IntersectS Third, Fourth, Fifth;
    
    printf("Thread got to pause\n");
    while (Pause != 0){
        //printf("%u", Pause); 
    }

    printf("Thread exited pause loop\n");

    while (1){
        pthread_mutex_lock(&CalcMutex);
        wordIndex = currentIndex;
        currentIndex++;
        pthread_mutex_unlock(&CalcMutex);

        if (wordIndex >= WordCount){
            printf("Thread exiting: wordIndex (%u) >= wordCount (%u)\n", wordIndex, WordCount);
            return 0; // Word list has been exhausted
        }

        Word *WordPtr = &(WordList[wordIndex]);
        if (WordPtr->Intersects.numIntersects < 4){
            continue;
        }

        for (int Word2 = 0; Word2 < WordPtr->Intersects.numIntersects; Word2++){
            Word *Word2Ptr = &(WordList[WordPtr->Intersects.IntersectArr[Word2]]);
            uint32_t word2Intersects = GetCommonWords(&WordPtr->Intersects, &Word2Ptr->Intersects, &Third);
            
            if (word2Intersects >=  3){
                for (int Word3 = 0; Word3 < word2Intersects; Word3++){
                    Word *Word3Ptr = &(WordList[Third.IntersectArr[Word3]]);
                    uint32_t word3Intersects = GetCommonWords(&Third, &WordPtr->Intersects, &Fourth);

                    if (word3Intersects >= 2){
                        for (int Word4 = 0; Word4 < word3Intersects; Word4++){
                            Word *Word4Ptr = &(WordList[Fourth.IntersectArr[Word4]]);
                            uint32_t word4Intersects = GetCommonWords(&Fourth, &Word4Ptr->Intersects, &Fifth);

                            if (word4Intersects){
                                //printf("word4inters: %u\n", word4Intersects);
                                for (int Word5 = 0; Word5 < word4Intersects; Word5++){
                                    Word *Word5Ptr = &(WordList[Fifth.IntersectArr[Word5]]);
                                    pthread_mutex_lock(&OutputMutex);
                                    ComboCount++;
                                    //printf("%u: %s, %s, %s, %s, %s\n", ComboCount, WordPtr->Chars, Word2Ptr->Chars, Word3Ptr->Chars, Word4Ptr->Chars, Word5Ptr->Chars);
                                    pthread_mutex_unlock(&OutputMutex);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

uint32_t GetCommonWords(IntersectS *IntersectsA, IntersectS *IntersectsB, IntersectS *out){ //will return number of common words
    uint32_t PivotA = 0;
    uint32_t PivotB = 0;
    out->numIntersects = 0;

    for (;;){
        if ((PivotA >= IntersectsA->numIntersects) || (PivotB >= IntersectsB->numIntersects)){
            return out->numIntersects; // options exhausted
        }

        if (IntersectsA->IntersectArr[PivotA] > IntersectsB->IntersectArr[PivotB]){
            PivotB++;
        }

        else if (IntersectsA->IntersectArr[PivotA] < IntersectsB->IntersectArr[PivotB]){
            PivotA++;
        }

        else{ //the number matches
            out->IntersectArr[out->numIntersects] = IntersectsA->IntersectArr[PivotA];
            PivotA++;
            PivotB++;
            out->numIntersects++;
        }
    }
}
