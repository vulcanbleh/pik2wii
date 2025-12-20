#ifndef REVOSDK_MEM_ALLOCATOR_H
#define REVOSDK_MEM_ALLOCATOR_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

// Forward declarations
typedef struct MEMAllocator MEMAllocator;
typedef struct MEMiHeapHead MEMiHeapHead;

//////////////////// ALLOCATOR TYPES ///////////////////

/// Alloc-type functions for memory areas.
typedef void* (*MEMAllocatorAllocFunc)(MEMAllocator* allocator, u32 size);

/// Free-type functions for memory areas.
typedef void (*MEMAllocatorFreeFunc)(MEMAllocator* allocator, void* block);

/**
 * @brief Pair structure for alloc and free functions for memory areas.
 *
 * @note Size: 0x8.
 */
typedef struct MEMAllocatorFuncs {
	MEMAllocatorAllocFunc allocFunc; // _00
	MEMAllocatorFreeFunc freeFunc;   // _04
} MEMAllocatorFuncs;

/**
 * @brief Memory allocator object.
 *
 * @note Size: 0x10.
 */
typedef struct MEMAllocator {
	const MEMAllocatorFuncs* funcs; // _00
	MEMiHeapHead* heap;             // _04
	u32 heapParam1;                 // _08
	u32 heapParam2;                 // _0C
} MEMAllocator;

////////////////////////////////////////////////////////
/////////////////// GLOBAL FUNCTIONS ///////////////////

void* MEMAllocFromAllocator(MEMAllocator* allocator, u32 size);
void MEMFreeToAllocator(MEMAllocator* allocator, void* block);

void MEMInitAllocatorForExpHeap(MEMAllocator* allocator, MEMiHeapHead* heap, s32 align);
void MEMInitAllocatorForFrmHeap(MEMAllocator* allocator, MEMiHeapHead* heap, s32 align);

MEMiHeapHead* MEMCreateExpHeapEx(void* start, s32 size);
void MEMDestroyExpHeap(MEMiHeapHead*);
void* MEMAllocFromExpHeapEx(MEMiHeapHead*, u32, s32);
void MEMFreeToExpHeap(MEMiHeapHead*, void*);
u32 MEMResizeForMBlockExpHeap(MEMiHeapHead*, void*, u32);
u32 MEMGetTotalFreeSizeForExpHeap(MEMiHeapHead*);
u32 MEMGetAllocatableSizeForExpHeapEx(MEMiHeapHead*, s32);
u32 MEMAdjustExpHeap(MEMiHeapHead*);

////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

#endif
