#ifndef _REVOSDK_OS_OSCACHE_H
#define _REVOSDK_OS_OSCACHE_H

#include "RevoSDK/OS/OSThread.h"
#include "RevoSDK/OS/OSUtil.h"
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif // ifdef __cplusplus

#define OS_CACHE_BASE 0xE0000000

///////// CACHE FUNCTIONS ////////
// Data cache functions.
void DCInvalidateRange(void* addr, u32 numBytes);
void DCFlushRange(const void* addr, u32 numBytes);
void DCStoreRange(void* addr, u32 numBytes);
void DCFlushRangeNoSync(void* addr, u32 numBytes);
void DCStoreRangeNoSync(void* addr, u32 numBytes);
void DCZeroRange(void* addr, u32 numBytes);

// Instruction cache functions.
void ICInvalidateRange(void* addr, u32 numBytes);
void ICFlashInvalidate();
void ICEnable();

// Locked cache functions.
void LCEnable();
void LCDisable();
void LCLoadBlocks(void* destTag, const void* srcAddr, u32 numBlocks);
void LCStoreBlocks(void* destAddr, void* srcTag, u32 numBlocks);
u32 LCStoreData(void* destAddr, void* srcAddr, u32 numBytes);
void LCQueueWait(u32 length);
u32 LCQueueLength();

// L2 cache functions.
void L2GlobalInvalidate();

// Unused/inlined in P2.
void DCTouchRange(void* addr, u32 numBytes);

void ICSync();
void ICDisable();
void ICFreeze();
void ICUnfreeze();
void ICBlockInvalidate(void* addr);


u32 LCLoadData(void* destAddr, void* srcAddr, u32 numBytes);
void LCFlushQueue();

void L2Enable();
void L2Disable();
void L2SetDataOnly(BOOL doDataOnly);
void L2SetWriteThrough(BOOL doWriteThrough);

//////////////////////////////////

#ifdef __cplusplus
};
#endif // ifdef __cplusplus

#endif
