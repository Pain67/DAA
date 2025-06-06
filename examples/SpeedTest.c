
#define DAA_REGION_SIZE 4 * 1024

#define DAA_IMPLEMENTATION
#include "../src/DAA.c"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define DAA_ALLOC_RANGE 128
#define DAA_MAX_INUM 10000

uint16_t RndList[DAA_MAX_INUM] = {0};
FILE* TargetFilePtr;

void TestLinearArena(size_t IN_Iteration, size_t IN_Rounds) {
    double TotalTime = 0;
    double AvgTime = 0;
    double MaxTime = 0;
    double MinTime = 100000;

    for (size_t X = 0; X < IN_Rounds; X++) {
        daaLinearArena LArena = {0};
        clock_t Begin = clock();
        for (uint32_t X = 0; X < IN_Iteration; X++) {
            void* Temp = daaLinearAlloc(&LArena, RndList[X]);
        }
        clock_t End = clock();
        double Spent = (double)(End - Begin) / CLOCKS_PER_SEC;

        daaLinearFree(&LArena);

        TotalTime += Spent;
        if (Spent < MinTime) { MinTime = Spent; }
        if (Spent > MaxTime) { MaxTime = Spent; }
    }

    AvgTime = TotalTime / IN_Rounds;

    printf(
        "    Linear Arena Allocation {Iteration: %d, Rounds: %d, Total Time: %f, Avg: %f, Min: %f, Max: %f}\n",
        IN_Iteration,
        IN_Rounds,
        TotalTime,
        AvgTime,
        MinTime,
        MaxTime
    );

    char Buffer[1024] = {0};
    sprintf(
        Buffer,
        "%s;%d; %d; %f; %f; %f; %f\n",
        "LinearArena",
        IN_Iteration,
        IN_Rounds,
        TotalTime,
        AvgTime,
        MinTime,
        MaxTime
    );
    fputs(Buffer, TargetFilePtr);
}

void TestSmartArena(size_t IN_Iteration, size_t IN_Rounds) {
    double TotalTime = 0;
    double AvgTime = 0;
    double MaxTime = 0;
    double MinTime = 100000;

    for (size_t X = 0; X < IN_Rounds; X++) {
        daaSmartArena SArena = {0};
        clock_t Begin = clock();
        for (uint32_t X = 0; X < IN_Iteration; X++) {
            void* Temp = daaSmartAlloc(&SArena, RndList[X]);
        }
        clock_t End = clock();
        double Spent = (double)(End - Begin) / CLOCKS_PER_SEC;

        daaSmartFreeArena(&SArena);

        TotalTime += Spent;
        if (Spent < MinTime) { MinTime = Spent; }
        if (Spent > MaxTime) { MaxTime = Spent; }
    }

    AvgTime = TotalTime / IN_Rounds;

    printf(
        "    Smart Arena Allocation {Iteration: %d, Rounds: %d, Total Time: %f, Avg: %f, Min: %f, Max: %f}\n",
        IN_Iteration,
        IN_Rounds,
        TotalTime,
        AvgTime,
        MinTime,
        MaxTime
    );

    char Buffer[1024] = {0};
    sprintf(
        Buffer,
        "%s;%d; %d; %f; %f; %f; %f\n",
        "SmartArena",
        IN_Iteration,
        IN_Rounds,
        TotalTime,
        AvgTime,
        MinTime,
        MaxTime
    );
    fputs(Buffer, TargetFilePtr);
}

void TestMalloc(size_t IN_Iteration, size_t IN_Rounds) {
    double TotalTime = 0;
    double AvgTime = 0;
    double MaxTime = 0;
    double MinTime = 100000;

    for (size_t X = 0; X < IN_Rounds; X++) {
        clock_t Begin = clock();
        for (uint32_t X = 0; X < IN_Iteration; X++) {
            void* Temp = malloc(RndList[X]);
        }
        clock_t End = clock();
        double Spent = (double)(End - Begin) / CLOCKS_PER_SEC;

        TotalTime += Spent;
        if (Spent < MinTime) { MinTime = Spent; }
        if (Spent > MaxTime) { MaxTime = Spent; }
    }

    AvgTime = TotalTime / IN_Rounds;

    printf(
        "    malloc Allocation {Iteration: %d, Rounds: %d, Total Time: %f, Avg: %f, Min: %f, Max: %f}\n",
        IN_Iteration,
        IN_Rounds,
        TotalTime,
        AvgTime,
        MinTime,
        MaxTime
    );

    char Buffer[1024] = {0};
    sprintf(
        Buffer,
        "%s;%d; %d; %f; %f; %f; %f\n",
        "malloc",
        IN_Iteration,
        IN_Rounds,
        TotalTime,
        AvgTime,
        MinTime,
        MaxTime
    );
    fputs(Buffer, TargetFilePtr);
}

// Number of elements in IterationList
#define INum 4
// Number of Rounds per Iteration
#define RNum 100

int main() {

    size_t IterationList[INum] = {
        10,
        10,
        10,
        DAA_MAX_INUM
    };

    // Generate a list of Random Number
    // Store them so all 3 type use the same sequence of numbers
    for (uint32_t X = 0; X < DAA_MAX_INUM; X++) {
        // Generate a random number in range of [1..DAA_ALLOC_RANGE]
        RndList[X] = (rand() % (DAA_ALLOC_RANGE + 1)) + 1;
    }

    // ---------------------------------------------------------------------------------

    TargetFilePtr = fopen("result.csv", "w+");
    fputs("Allocator;Iterations; Rounds; Total Time; Avg; Min; Max\n", TargetFilePtr);

    // ---------------------------------------------------------------------------------
    printf("----- Random Size Allocation -----\n");
    for (size_t X = 0; X < INum; X++) {
        printf("Random size allocation (%d iteration)\n", IterationList[X]);
        TestMalloc(IterationList[X], RNum);
        TestLinearArena(IterationList[X], RNum);
        TestSmartArena(IterationList[X], RNum);
        printf("\n");
    }

    // ---------------------------------------------------------------------------------

    for (uint32_t X = 0; X < DAA_MAX_INUM; X++) { RndList[X] = 1; }
    printf("----- Fixed Size Allocation (1) -----\n");
    for (size_t X = 0; X < INum; X++) {
        printf("Fixed size allocation (1 bytes - %d iteration)\n", IterationList[X]);
        TestMalloc(IterationList[X], RNum);
        TestLinearArena(IterationList[X], RNum);
        TestSmartArena(IterationList[X], RNum);
        printf("\n");
    }
    // ---------------------------------------------------------------------------------
    for (uint32_t X = 0; X < DAA_MAX_INUM; X++) { RndList[X] = 2; }
    printf("----- Fixed Size Allocation (2) -----\n");
    for (size_t X = 0; X < INum; X++) {
        printf("Fixed size allocation (2 bytes - %d iteration)\n", IterationList[X]);
        TestMalloc(IterationList[X], RNum);
        TestLinearArena(IterationList[X], RNum);
        TestSmartArena(IterationList[X], RNum);
        printf("\n");
    }
    // ---------------------------------------------------------------------------------
    for (uint32_t X = 0; X < DAA_MAX_INUM; X++) { RndList[X] = 4; }
    printf("----- Fixed Size Allocation (4) -----\n");
    for (size_t X = 0; X < INum; X++) {
        printf("Fixed size allocation (4 bytes - %d iteration)\n", IterationList[X]);
        TestMalloc(IterationList[X], RNum);
        TestLinearArena(IterationList[X], RNum);
        TestSmartArena(IterationList[X], RNum);
        printf("\n");
    }
    // ---------------------------------------------------------------------------------
    for (uint32_t X = 0; X < DAA_MAX_INUM; X++) { RndList[X] = 8; }
    printf("----- Fixed Size Allocation (8) -----\n");
    for (size_t X = 0; X < INum; X++) {
        printf("Fixed size allocation (8 bytes - %d iteration)\n", IterationList[X]);
        TestMalloc(IterationList[X], RNum);
        TestLinearArena(IterationList[X], RNum);
        TestSmartArena(IterationList[X], RNum);
        printf("\n");
    }
    // ---------------------------------------------------------------------------------

    fclose(TargetFilePtr);

    // ---------------------------------------------------------------------------------

    return 0;
}
