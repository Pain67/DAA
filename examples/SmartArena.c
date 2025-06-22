#define DAA_IMPLEMENTATION
#include "DAA.h"
#include <stdlib.h>

#define ITERATIONS 100

int main() {

    const int MAX = 64;

    daaSmartArena SArena = {0};

    void* PtrList[ITERATIONS] = {0};

    for (int X = 0; X < ITERATIONS; X++) {
        PtrList[X] = daaSmartAlloc(&SArena, rand() % (MAX + 1));
    }

    for (int X = 0; X < ITERATIONS; X++) {
        if (PtrList[X] == NULL) {continue;}
        if ((rand() % 3) == 0) {
            daaSmartFree(&SArena, PtrList[X]);
        }
    }


    daaPrintSmartArena(&SArena);
    daaSmartFreeArena(&SArena);

    return 0;
}
