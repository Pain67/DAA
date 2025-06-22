#include "DAA.h"

#include <stdio.h>
#include <stdlib.h>

// -------------------------------------------------------------------------------------------------
// Implementations
// -------------------------------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

// Panic
void daaPANIC(const char* IN_Msg) {
    printf("----- Arena Panic -----\n");
    printf("Reson: %s\n", IN_Msg);
    printf("Terminating with non-zero exit code [67]\n");
    printf("----- -----\n");
    exit(67);
}

// LINEAR ARENA
daaLinearArena* daaCreateLinearArena(size_t IN_RegionSize) {
    if (IN_RegionSize < 8) {
        daaPANIC("Failed to create new LinearArena. Requested region size is below minimum limit.");
    }

    daaLinearArena* NewArena = malloc(sizeof(daaLinearArena));

    NewArena->REGION_SIZE = IN_RegionSize;
    NewArena->FirstRegion = NULL;
    NewArena->CurrRegion = NULL;
    NewArena->RegionCount = 0;

    return NewArena;
}

daaLinearRegion* daaInitLinearRegion(size_t IN_RegionSize) {
    daaLinearRegion* Result = malloc(sizeof(daaLinearRegion));
    if (Result == NULL) {
        daaPANIC("Failed to allocate memory for new region.");
    }
    Result->Capacity = IN_RegionSize;
    Result->Size = 0;
    Result->AllocNum = 0;
    Result->Data = malloc(IN_RegionSize);
    if (Result->Data  == NULL) {
        daaPANIC("Failed to allocate memory for region data.");
    }
    Result->Next = NULL;

    return Result;
}

void* daaLinearAlloc(daaLinearArena* IN_Arena, size_t IN_AllocSize) {
    if (IN_Arena == NULL) { daaPANIC("Failed to allocate, Arena is NULL"); }
    if (IN_AllocSize > IN_Arena->REGION_SIZE) { daaPANIC("Failed to allocate, Requested size is above limit."); }

    if (IN_Arena->FirstRegion == NULL) {
        IN_Arena->FirstRegion = daaInitLinearRegion(IN_Arena->REGION_SIZE);
        IN_Arena->CurrRegion = IN_Arena->FirstRegion;
        IN_Arena->RegionCount = 1;
    }

    if ((IN_Arena->CurrRegion->Size + IN_AllocSize) <= IN_Arena->CurrRegion->Capacity) {
        void* Result = &IN_Arena->CurrRegion->Data[IN_Arena->CurrRegion->Size];
        IN_Arena->CurrRegion->Size += IN_AllocSize;
        IN_Arena->CurrRegion->AllocNum++;
        return Result;
    }

    IN_Arena->CurrRegion->Next = daaInitLinearRegion(IN_Arena->REGION_SIZE);
    IN_Arena->RegionCount++;
    IN_Arena->CurrRegion = IN_Arena->CurrRegion->Next;
    void* Result = &IN_Arena->CurrRegion->Data[0];
    IN_Arena->CurrRegion->Size += IN_AllocSize;
    IN_Arena->CurrRegion->AllocNum++;
    return Result;
}

void daaLinearFree(daaLinearArena* IN_Arena) {
    daaLinearRegion* CurrRegion = IN_Arena->FirstRegion;
    while (CurrRegion != NULL) {
        daaLinearRegion* Temp = CurrRegion->Next;
        free(CurrRegion->Data);
        free(CurrRegion);
        CurrRegion = Temp;
    }
    IN_Arena->FirstRegion = NULL;
    IN_Arena->CurrRegion = NULL;
    IN_Arena->RegionCount = 0;
}

void daaPrintLinearArenaRegion(daaLinearRegion* IN_Region) {
    printf(
        "Region { Capacity: %zu; Size: %zu; Allocations: %zu; Data addr: %p; }\n",
        IN_Region->Capacity,
        IN_Region->Size,
        IN_Region->AllocNum,
        IN_Region->Data
    );
}

void daaPrintLinearArena(daaLinearArena* IN_Arena) {
    printf("Linear Arena {\n");
    daaLinearRegion* CurrRegion = IN_Arena->FirstRegion;
    size_t Index = 0;
    if (CurrRegion == NULL) {
        printf("    NULL\n");
    }
    else {
        while (CurrRegion != NULL) {
            printf("    [%zu] ", Index);
            daaPrintLinearArenaRegion(CurrRegion);
            CurrRegion = CurrRegion->Next;
            Index++;
        }
    }
    printf("}\n");
}


void daaFreeLinearArena(daaLinearArena* IN_Arena) { free(IN_Arena); }

// SMART ARENA
daaSmartArena* daaCreateSmartArena(size_t IN_RegionSize) {
    if (IN_RegionSize < 8) {
        daaPANIC("Failed to create new SmartArena. Requested region size is below minimum limit.");
    }

    daaSmartArena* NewArena = malloc(sizeof(daaSmartArena));

    NewArena->REGION_SIZE = IN_RegionSize;
    NewArena->FirstRegion = NULL;
    NewArena->CurrRegion = NULL;
    NewArena->RegionCount = 0;

    return NewArena;
}

daaSmartRegion* daaInitSmartRegion(size_t IN_RegionSize) {
    daaSmartRegion* Result = malloc(sizeof(daaSmartRegion));
    if (Result == NULL) {
        daaPANIC("Failed to allocate memory for region.");
    }
    Result->Capacity = IN_RegionSize;
    Result->AllocNum = 0;
    Result->Data = malloc(IN_RegionSize);
    if (Result->Data == NULL) {
        daaPANIC("Failed to allocate memory for region data.");
    }
    Result->Next = NULL;
    Result->Prev = NULL;

    Result->AllocList = malloc(sizeof(daaSmartRegionAllocEntry));
    if (Result->AllocList == NULL) {
        daaPANIC("Failed to allocate memory for region allocation list.");
    }
    Result->AllocList->Size = IN_RegionSize;
    Result->AllocList->isAlloc = 0;
    Result->AllocList->Next = NULL;
    Result->FreeCount = 1;

    return Result;
}

void* daaSmartRegionAlloc(daaSmartRegion* IN_Region, size_t IN_AllocSize) {
    void* Result = &IN_Region->Data[0];

    daaSmartRegionAllocEntry* CurrAlloc = IN_Region->AllocList;
    while (CurrAlloc != NULL) {
        if (CurrAlloc->isAlloc == 0) {
            if (CurrAlloc->Size > IN_AllocSize) {
                daaSmartRegionAllocEntry* NewAlloc = malloc(sizeof(daaSmartRegionAllocEntry));
                if (NewAlloc == NULL) {
                    daaPANIC("Failed to allocate memory for  new region.");
                }
                NewAlloc->Size = CurrAlloc->Size - IN_AllocSize;
                NewAlloc->isAlloc = 0;
                NewAlloc->Next = CurrAlloc->Next;
                NewAlloc->Prev = CurrAlloc;

                CurrAlloc->Size = IN_AllocSize;
                CurrAlloc->isAlloc = 1;
                CurrAlloc->Next = NewAlloc;
                IN_Region->AllocNum++;

                return Result;
            }
            else if (CurrAlloc->Size == IN_AllocSize) {
                CurrAlloc->isAlloc = 1;
                IN_Region->AllocNum++;
                IN_Region->FreeCount--;

                return Result;
            }
        }
        Result += CurrAlloc->Size;
        CurrAlloc = CurrAlloc->Next;
    }

    return NULL;
}

bool daaSmartRegionFree(daaSmartRegion* IN_Region, void* IN_Ptr) {
    daaSmartRegionAllocEntry* CurrAlloc = IN_Region->AllocList;
    void* BasePtr = &IN_Region->Data[0];

    while (CurrAlloc != NULL) {
        if (CurrAlloc->isAlloc == 1) {
            if (BasePtr == IN_Ptr)  {
                CurrAlloc->isAlloc = 0;
                bool isMerged = false;

                // Check Next Region
                if (CurrAlloc->Next != NULL) {
                    daaSmartRegionAllocEntry* Next = CurrAlloc->Next;
                    if (Next->isAlloc == 0) {
                        daaSmartRegionAllocEntry* Temp = Next->Next;
                        CurrAlloc->Size += Next->Size;
                        free(Next);
                        CurrAlloc->Next = Temp;
                        isMerged = true;
                    }
                }

                // Check PrevRegion
                if (CurrAlloc->Prev != NULL) {
                    daaSmartRegionAllocEntry* Prev = CurrAlloc->Prev;
                    if (Prev->isAlloc == 0) {
                        daaSmartRegionAllocEntry* Temp = Prev->Prev;
                        CurrAlloc->Size += Prev->Size;
                        CurrAlloc->Prev = Temp;
                        free(Prev);
                        if (Temp != NULL) { Temp->Next = CurrAlloc; }
                        else { IN_Region->AllocList = CurrAlloc; }
                        isMerged = true;
                    }
                }

                IN_Region->AllocNum--;
                if (!isMerged) { IN_Region->FreeCount++; }

                return true;
            }
        }
        BasePtr += CurrAlloc->Size;
        CurrAlloc = CurrAlloc->Next;
    }
    return false;
}

void* daaSmartAlloc(daaSmartArena* IN_Arena, size_t IN_AllocSize) {
    if (IN_Arena == NULL) { daaPANIC("Failed to allocate, Arena is NULL"); }
    if (IN_AllocSize > IN_Arena->REGION_SIZE) { daaPANIC("Failed to allocate, Requested size is above limit."); }

    if (IN_AllocSize <= 0) { return NULL; }

    if (IN_Arena->FirstRegion == NULL) {
        IN_Arena->FirstRegion = daaInitSmartRegion(IN_Arena->REGION_SIZE);
        IN_Arena->CurrRegion = IN_Arena->FirstRegion;
        IN_Arena->RegionCount = 1;
    }

    daaSmartRegion* CurrRegion = IN_Arena->FirstRegion;
    while (CurrRegion != NULL) {
        if (CurrRegion->FreeCount > 0) {
            void* Result = daaSmartRegionAlloc(CurrRegion, IN_AllocSize);
            if (Result != NULL) { return Result; }
        }
        CurrRegion = CurrRegion->Next;
    }

    // Wee need a new Region
    IN_Arena->CurrRegion->Next = daaInitSmartRegion(IN_Arena->REGION_SIZE);
    daaSmartRegion* Temp = IN_Arena->CurrRegion->Next;
    Temp->Prev = IN_Arena->CurrRegion;
    IN_Arena->CurrRegion = IN_Arena->CurrRegion->Next;
    IN_Arena->RegionCount++;

    void* Result = daaSmartRegionAlloc(IN_Arena->CurrRegion, IN_AllocSize);

    return Result;
}

void daaSmartFree(daaSmartArena* IN_Arena, void* IN_Ptr) {
    if (IN_Ptr == NULL) { return; }

    daaSmartRegion* CurrRegion = IN_Arena->FirstRegion;
    while (CurrRegion != NULL) {
        if (daaSmartRegionFree(CurrRegion, IN_Ptr)) {
            return;
        }
        CurrRegion = CurrRegion->Next;
    }

    daaPrintSmartArena(IN_Arena);
    printf("%p\n", IN_Ptr);
    daaPANIC("Attempting to free an unknown pointer");
}

void daaSmartFreeArena(daaSmartArena* IN_Arena) {
    daaSmartRegion* CurrRegion = IN_Arena->FirstRegion;
    while (CurrRegion != NULL) {
        daaSmartRegion* Temp = CurrRegion->Next;

        daaSmartRegionAllocEntry* CurrAlloc = CurrRegion->AllocList;
        while (CurrAlloc != NULL) {
            daaSmartRegionAllocEntry* TempAlloc = CurrAlloc->Next;
            free(CurrAlloc);
            CurrAlloc = TempAlloc;
        }

        free(CurrRegion->Data);
        free(CurrRegion);
        CurrRegion = Temp;
    }
    IN_Arena->FirstRegion = NULL;
    IN_Arena->CurrRegion = NULL;
    IN_Arena->RegionCount = 0;

    free(IN_Arena);
}

void daaPrintSmartRegion(daaSmartRegion* IN_Region) {
    size_t Offset = 0;
    void* Base = &IN_Region->Data[0];
    daaSmartRegionAllocEntry* CurrAlloc = IN_Region->AllocList;

    printf("SmartRegion {\n");
    printf("Allocations: %zu\n", IN_Region->AllocNum);
    printf("Free Count: %zu\n", IN_Region->FreeCount);
    while (CurrAlloc != NULL) {
        printf(
            "    Allocation { Offset: %zu; Size: %zu; Type: %d; Addr: %p }\n",
            Offset,
            CurrAlloc->Size,
            CurrAlloc->isAlloc,
            (Base + Offset)
        );
        Offset += CurrAlloc->Size;
        CurrAlloc = CurrAlloc->Next;
    }
    printf("}\n");
}

void daaPrintSmartRegionMap(daaSmartRegion* IN_Region, size_t IN_Cols) {
    char CurrChar[2] = { 0 };
    size_t Pos = 0;

    daaSmartRegionAllocEntry* CurrAlloc = IN_Region->AllocList;

    printf("SmartRegionMap {\n");
    while (CurrAlloc != NULL) {
        if (CurrAlloc->isAlloc == 0) { CurrChar[0] = '.'; }
        else { CurrChar[0] = '#'; }
        for (size_t X = 0; X < CurrAlloc->Size; X++) {
            printf("%s", CurrChar);
            Pos++;
            if (Pos >= IN_Cols) {
                printf("\n");
                Pos = 0;
            }
        }
        CurrAlloc = CurrAlloc->Next;
    }
    printf("}\n");
}

void daaPrintSmartArena(daaSmartArena* IN_Arena) {
    printf("------------------------------------------------------\n");
    printf("Smart Arena: \n\n");
    printf("Region: %zu;\n", IN_Arena->RegionCount);
    daaSmartRegion* CurrRegion = IN_Arena->FirstRegion;
    if (CurrRegion == NULL) {
        printf("NULL\n");
    }
    else {
        while (CurrRegion != NULL) {
            daaPrintSmartRegion(CurrRegion);
            CurrRegion = CurrRegion->Next;
        }
    }
    printf("\n");
    printf("------------------------------------------------------\n");
}

#ifdef __cplusplus
}
#endif
