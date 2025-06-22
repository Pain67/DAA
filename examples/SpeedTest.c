#define DAA_IMPLEMENTATION
#include "DAA.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

// Max number of bytes to allocate in random
#define DAA_ALLOC_RANGE 128
// Number of Allocation per allocator
#define DAA_ITERATION_NUM 100000
// Number of Rounds per Iteration
#define RNum 100
// Temp Buffer Size (For git result)
#define TEMP_BUFFER_SIZE 1024

uint16_t RndList[DAA_ITERATION_NUM] = {0};
FILE* TargetFilePtr;
FILE* GitFilePtr;
char TempBuffer[TEMP_BUFFER_SIZE] = {0};
const size_t REGION_SIZE = 4 * 1024;

void TestLinearArena(size_t IN_Iteration, size_t IN_Rounds) {
    double TotalTime = 0;
    double AvgTime = 0;
    double MaxTime = 0;
    double MinTime = 100000;

    for (size_t X = 0; X < IN_Rounds; X++) {
        daaLinearArena* LArena = daaCreateLinearArena(REGION_SIZE);
        clock_t Begin = clock();
        for (uint32_t X = 0; X < IN_Iteration; X++) {
            void* Temp = daaLinearAlloc(LArena, RndList[X]);
        }
        clock_t End = clock();
        double Spent = (double)(End - Begin) / CLOCKS_PER_SEC;

        daaLinearFree(LArena);
        daaFreeLinearArena(LArena);

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

    sprintf(
        TempBuffer,
        "%s;%d; %d; %f; %f; %f; %f\n",
        "LinearArena",
        IN_Iteration,
        IN_Rounds,
        TotalTime,
        AvgTime,
        MinTime,
        MaxTime
    );
    fputs(TempBuffer, TargetFilePtr);
    memset(TempBuffer, 0, TEMP_BUFFER_SIZE);

    sprintf(
        TempBuffer,
        "| LinearArena | %f | %f | %f | %f |\n",
        TotalTime,
        AvgTime,
        MinTime,
        MaxTime
    );
    fputs(TempBuffer, GitFilePtr);
    memset(TempBuffer, 0, TEMP_BUFFER_SIZE);
}

void TestSmartArena(size_t IN_Iteration, size_t IN_Rounds) {
    double TotalTime = 0;
    double AvgTime = 0;
    double MaxTime = 0;
    double MinTime = 100000;

    for (size_t X = 0; X < IN_Rounds; X++) {
        daaSmartArena* SArena = daaCreateSmartArena(REGION_SIZE);
        clock_t Begin = clock();
        for (uint32_t X = 0; X < IN_Iteration; X++) {
            void* Temp = daaSmartAlloc(SArena, RndList[X]);
        }
        clock_t End = clock();
        double Spent = (double)(End - Begin) / CLOCKS_PER_SEC;

        daaSmartFreeArena(SArena);

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

    sprintf(
        TempBuffer,
        "%s;%d; %d; %f; %f; %f; %f\n",
        "SmartArena",
        IN_Iteration,
        IN_Rounds,
        TotalTime,
        AvgTime,
        MinTime,
        MaxTime
    );
    fputs(TempBuffer, TargetFilePtr);
    memset(TempBuffer, 0, TEMP_BUFFER_SIZE);

    sprintf(
        TempBuffer,
        "| SmartArena | %f | %f | %f | %f |\n",
        TotalTime,
        AvgTime,
        MinTime,
        MaxTime
    );
    fputs(TempBuffer, GitFilePtr);
    memset(TempBuffer, 0, TEMP_BUFFER_SIZE);
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

    sprintf(
        TempBuffer,
        "%s;%d; %d; %f; %f; %f; %f\n",
        "malloc",
        IN_Iteration,
        IN_Rounds,
        TotalTime,
        AvgTime,
        MinTime,
        MaxTime
    );
    fputs(TempBuffer, TargetFilePtr);
    memset(TempBuffer, 0, TEMP_BUFFER_SIZE);

    sprintf(
        TempBuffer,
        "| malloc | %f | %f | %f | %f |\n",
        TotalTime,
        AvgTime,
        MinTime,
        MaxTime
    );
    fputs(TempBuffer, GitFilePtr);
    memset(TempBuffer, 0, TEMP_BUFFER_SIZE);
}


int main() {

    // Generate a list of Random Number
    // Store them so all 3 type use the same sequence of numbers
    for (uint32_t X = 0; X < DAA_ITERATION_NUM; X++) {
        // Generate a random number in range of [1..DAA_ALLOC_RANGE]
        RndList[X] = (rand() % (DAA_ALLOC_RANGE + 1)) + 1;
    }

    // ---------------------------------------------------------------------------------

    TargetFilePtr = fopen("result.csv", "w+");
    GitFilePtr = fopen("git_result.txt", "w+");
    fputs("Allocator;Iterations; Rounds; Total Time; Avg; Min; Max\n", TargetFilePtr);

    // ---------------------------------------------------------------------------------
    printf("----- Random Size Allocation -----\n");
    printf("Random size allocation (%d iteration)\n", DAA_ITERATION_NUM);

    sprintf(
        TempBuffer,
        "### Random Size Allocation (1 - %d bytes) - %d allocations - %d rounds per allocator\n"
        "| Allocator | Total Time (sec) | AvgTime (sec) | Min (sec) | Max (sec) |\n"
        "| --------- |:----------------:|:-------------:|:---------:|:---------:|\n",
        DAA_ALLOC_RANGE,
        DAA_ITERATION_NUM,
        RNum
    );
    fputs(TempBuffer, GitFilePtr);
    memset(TempBuffer, 0, TEMP_BUFFER_SIZE);

    TestMalloc(DAA_ITERATION_NUM, RNum);
    TestLinearArena(DAA_ITERATION_NUM, RNum);
    TestSmartArena(DAA_ITERATION_NUM, RNum);


    printf("\n");
    fputs("\n", GitFilePtr);

    // ---------------------------------------------------------------------------------
    for (uint32_t X = 0; X < DAA_ITERATION_NUM; X++) { RndList[X] = 1; }
    printf("----- Fixed Size Allocation (1) -----\n");
    printf("Fixed size allocation (1 bytes - %d iteration)\n", DAA_ITERATION_NUM);

    sprintf(
        TempBuffer,
        "### Fixed Size Allocation (1 bytes) - %d allocations - %d rounds per allocator\n"
        "| Allocator | Total Time (sec) | AvgTime (sec) | Min (sec) | Max (sec) |\n"
        "| --------- |:----------------:|:-------------:|:---------:|:---------:|\n",
            DAA_ITERATION_NUM,
            RNum
    );
    fputs(TempBuffer, GitFilePtr);
    memset(TempBuffer, 0, TEMP_BUFFER_SIZE);

    TestMalloc(DAA_ITERATION_NUM, RNum);
    TestLinearArena(DAA_ITERATION_NUM, RNum);
    TestSmartArena(DAA_ITERATION_NUM, RNum);

    printf("\n");
    fputs("\n", GitFilePtr);

    // ---------------------------------------------------------------------------------
    for (uint32_t X = 0; X < DAA_ITERATION_NUM; X++) { RndList[X] = 2; }
    printf("----- Fixed Size Allocation (2) -----\n");
    printf("Fixed size allocation (2 bytes - %d iteration)\n", DAA_ITERATION_NUM);

    sprintf(
        TempBuffer,
        "### Fixed Size Allocation (2 bytes) - %d allocations - %d rounds per allocator\n"
        "| Allocator | Total Time (sec) | AvgTime (sec) | Min (sec) | Max (sec) |\n"
        "| --------- |:----------------:|:-------------:|:---------:|:---------:|\n",
        DAA_ITERATION_NUM,
        RNum
    );
    fputs(TempBuffer, GitFilePtr);
    memset(TempBuffer, 0, TEMP_BUFFER_SIZE);

    TestMalloc(DAA_ITERATION_NUM, RNum);
    TestLinearArena(DAA_ITERATION_NUM, RNum);
    TestSmartArena(DAA_ITERATION_NUM, RNum);

    printf("\n");
    fputs("\n", GitFilePtr);

    // ---------------------------------------------------------------------------------
    for (uint32_t X = 0; X < DAA_ITERATION_NUM; X++) { RndList[X] = 4; }
    printf("----- Fixed Size Allocation (4) -----\n");
    printf("Fixed size allocation (4 bytes - %d iteration)\n", DAA_ITERATION_NUM);

    sprintf(
        TempBuffer,
        "### Fixed Size Allocation (4 bytes) - %d allocations - %d rounds per allocator\n"
        "| Allocator | Total Time (sec) | AvgTime (sec) | Min (sec) | Max (sec) |\n"
        "| --------- |:----------------:|:-------------:|:---------:|:---------:|\n",
        DAA_ITERATION_NUM,
        RNum
    );
    fputs(TempBuffer, GitFilePtr);
    memset(TempBuffer, 0, TEMP_BUFFER_SIZE);

    TestMalloc(DAA_ITERATION_NUM, RNum);
    TestLinearArena(DAA_ITERATION_NUM, RNum);
    TestSmartArena(DAA_ITERATION_NUM, RNum);

    printf("\n");
    fputs("\n", GitFilePtr);

    // ---------------------------------------------------------------------------------
    for (uint32_t X = 0; X < DAA_ITERATION_NUM; X++) { RndList[X] = 8; }
    printf("----- Fixed Size Allocation (8) -----\n");
    printf("Fixed size allocation (8 bytes - %d iteration)\n", DAA_ITERATION_NUM);

    sprintf(
        TempBuffer,
        "### Fixed Size Allocation (8 bytes) - %d allocations - %d rounds per allocator\n"
        "| Allocator | Total Time (sec) | AvgTime (sec) | Min (sec) | Max (sec) |\n"
        "| --------- |:----------------:|:-------------:|:---------:|:---------:|\n",
        DAA_ITERATION_NUM,
        RNum
    );
    fputs(TempBuffer, GitFilePtr);
    memset(TempBuffer, 0, TEMP_BUFFER_SIZE);

    TestMalloc(DAA_ITERATION_NUM, RNum);
    TestLinearArena(DAA_ITERATION_NUM, RNum);
    TestSmartArena(DAA_ITERATION_NUM, RNum);

    printf("\n");
    fputs("\n", GitFilePtr);

    // ---------------------------------------------------------------------------------

    fclose(TargetFilePtr);

    // ---------------------------------------------------------------------------------

    return 0;
}
