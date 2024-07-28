#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdint.h>

#define MaxIntersects 20000


typedef struct WordStruct{
    char Chars[64];
    uint32_t LetterMask;
    uint32_t Intersects[MaxIntersects];

} Word;
