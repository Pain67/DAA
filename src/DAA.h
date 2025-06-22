#ifndef DAA_DAA_H
#define DAA_DAA_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// -------------------------------------------------------------------------------------------------
// Types
// -------------------------------------------------------------------------------------------------

typedef struct daaLinearRegion {
    size_t Capacity;
    size_t Size;
    size_t AllocNum;
    uint16_t* Data;
    void* Next;
} daaLinearRegion;

typedef struct daaLinearArena {
    size_t REGION_SIZE;
    daaLinearRegion* FirstRegion;
    daaLinearRegion* CurrRegion;
    size_t RegionCount;
} daaLinearArena;

typedef struct daaSmartRegionAllocEntry {
    size_t Size;
    uint16_t isAlloc;
    void* Next;
    void* Prev;
} daaSmartRegionAllocEntry;

typedef struct daaSmartRegion {
    size_t Capacity;
    size_t AllocNum;
    daaSmartRegionAllocEntry* AllocList;
    uint16_t* Data;
    void* Next;
    void* Prev;
} daaSmartRegion;

typedef struct daaSmartArena{
    size_t REGION_SIZE;
    daaSmartRegion* FirstRegion;
    daaSmartRegion* CurrRegion;
    size_t RegionCount;
} daaSmartArena;

// -------------------------------------------------------------------------------------------------
// Headers
// -------------------------------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

// Panic
void daaPANIC(const char *IN_Msg);

// Linear Arena
daaLinearArena* daaCreateLinearArena(size_t IN_RegionSize);

daaLinearRegion *daaInitLinearRegion(size_t IN_RegionSize);

void *daaLinearAlloc(daaLinearArena *IN_Arena, size_t IN_AllocSize);

void daaLinearFree(daaLinearArena *IN_Arena);

void daaPrintLinearArenaRegion(daaLinearRegion *IN_Region);

void daaPrintLinearArena(daaLinearArena *IN_Arena);

void daaFreeLinearArena(daaLinearArena* IN_Arena);

// Smart Arena
daaSmartArena* daaCreateSmartArena(size_t IN_RegionSize);

daaSmartRegion *daaInitSmartRegion(size_t IN_RegionSize);

void *daaSmartRegionAlloc(daaSmartRegion *IN_Region, size_t IN_AllocSize);

bool daaSmartRegionFree(daaSmartRegion *IN_Region, void *IN_Ptr);

void *daaSmartAlloc(daaSmartArena *IN_Arena, size_t IN_AllocSize);

void daaSmartFree(daaSmartArena *IN_Arena, void *IN_Ptr);

void daaSmartFreeArena(daaSmartArena *IN_Arena);

void daaPrintSmartRegion(daaSmartRegion *IN_Region);

void daaPrintSmartRegionMap(daaSmartRegion *IN_Region, size_t IN_Cols);

void daaPrintSmartArena(daaSmartArena *IN_Arena);

#ifdef __cplusplus
}
#endif

#endif //DAA_DAA_H
