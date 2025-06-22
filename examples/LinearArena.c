#define DAA_IMPLEMENTATION
#include "DAA.h"
#include <stdlib.h>

int main() {

    const int MAX = 64;
    const int REGION_SIZE = 256;

    daaLinearArena* LArena = daaCreateLinearArena(REGION_SIZE);

    for (int X = 0; X < 1500; X++) {
        daaLinearAlloc(LArena, rand() % (MAX + 1));
    }


    daaPrintLinearArena(LArena);
    daaLinearFree(LArena);
    daaFreeLinearArena(LArena);

    return 0;
}
