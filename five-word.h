#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdint.h>

#define MaxWords 15000
#define MaxIntersects 10000
#define MaxWordLength 64
#define MaxThreads 64

typedef struct WordStruct{
    char Chars[MaxWordLength];
    uint32_t LetterMask;
    uint32_t Intersects[MaxIntersects];
    uint32_t numIntersects;

} Word;

Word *AllWords;
Word *WordList; // All valid words
uint32_t WordCount;
uint32_t ComboCount;
pthread_mutex_t CalcMutex;
pthread_mutex_t OutputMutex;
pthread_t Threads[128];
uint8_t Pause;
uint32_t currentIndex; // Index of word currently being worked on


uint32_t LoadWordFile(FILE *WordFile);
void GetIntersects(Word *wordList, uint32_t numWords);
void *Calc(void* arg);
