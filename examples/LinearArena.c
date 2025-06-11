#define DAA_IMPLEMENTATION
#include "DAA.h"

int main() {

    const int MAX = 64;

    daaLinearArena LArena = {0};

    for (int X = 0; X < 1500; X++) {
        daaLinearAlloc(&LArena, rand() % (MAX + 1));
    }


    daaPrintLinearArena(&LArena);
    daaLinearFree(&LArena);

    return 0;
}
