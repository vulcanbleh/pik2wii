#include "RevoSDK/OS/OSCache.h"
#include "RevoSDK/OS/OSInterrupt.h"
#include "RevoSDK/OS/OSThread.h"
#include "RevoSDK/ar.h"
#include "RevoSDK/os.h"
#include "stl/mem.h"

BOOL __ARQ_init_flag;
u32 __ARQChunkSize;

ARQCallback __ARQCallbackLo;
ARQCallback __ARQCallbackHi;

ARQRequest* __ARQRequestPendingLo;
ARQRequest* __ARQRequestPendingHi;

ARQRequest* __ARQRequestQueueLo;
ARQRequest* __ARQRequestQueueHi;

u32 __ARH_BaseAdr;
u32 __AR_Size;
u32 __AR_InternalSize;
BOOL __AR_init_flag;
void (*__AR_Callback)();

u32 __ARALT_AramStartAdr = __AR_ARAM_BASE_MEMORY_TOP;
u32 __ARH_MemoryTop = __AR_ARAM_BASE_MEMORY_TOP;

// at some point we might want to make these parameters void*
void ARStartDMA(u32 type, u32 mainmem_addr, u32 aram_addr, u32 length)
{
	OSDisableScheduler();
	if (type == 0) {
		aram_addr += __ARH_BaseAdr;
		DCInvalidateRange((void*)mainmem_addr, length);

		(void)memcpy((void*)aram_addr, (void*)mainmem_addr, length);

		DCFlushRange((void*)aram_addr, length);
	} else if (type == 1) {
		mainmem_addr += __ARH_BaseAdr;
		DCFlushRange((void*)mainmem_addr, length);

		(void)memcpy((void*)aram_addr, (void*)mainmem_addr, length);

		DCFlushRange((void*)aram_addr, length);
	}

	OSEnableScheduler();

	if (__AR_Callback) {
		__AR_Callback();
	}
}

u32 ARAlloc(u32 length)
{
	BOOL level;
	u32 top;

	level = OSDisableInterrupts();
	top   = __ARH_MemoryTop;
	(void)OSRestoreInterrupts(level);

	__ARH_MemoryTop += length;

	return top - __ARH_BaseAdr;
}

u32 ARInit(u32* stack_index_addr, u32 num_entries)
{
	BOOL level;

	u32 r29;
	u32 r30;
	u32 diff;

	if (__AR_init_flag) {
		return ARGetBaseAddress();
	}

	level = OSDisableInterrupts();

	__AR_Callback  = nullptr;
	__AR_init_flag = TRUE;

	(void)OSRestoreInterrupts(level);

	r30 = __ARALT_AramStartAdr;

	r29 = r30 + 0x1300000U;

	__ARH_MemoryTop = ARGetBaseAddress() + r30;
	__ARH_BaseAdr = r30;

	diff = r29 - r30;

	OSReport("ARInit : Dummy ARAM enabled (RVL), area %p -> %p (size 0x%x)\n", __ARH_BaseAdr, r29, r29 - r30);

	
	__AR_InternalSize         = diff;
	__AR_Size = __AR_InternalSize;

	return ARGetBaseAddress();
}

u32 ARGetBaseAddress()
{
	return __AR_ARAM_USR_BASE_ADDR;
}

u32 ARGetSize()
{
	return __AR_InternalSize;
}

void generate();

void __ARQPopTaskQueueHi()
{
	ARQRequest* low;

	if (!__ARQRequestPendingLo && __ARQRequestQueueLo) {
		__ARQRequestPendingLo = __ARQRequestQueueLo;
		__ARQRequestQueueLo   = __ARQRequestPendingLo->next;
	}

	low = __ARQRequestPendingLo;
	if (!low) {
		return;
	}

	if (low->length <= __ARQChunkSize) {
		if (low->type == 0) {
			ARStartDMA(low->type, low->source, low->dest, low->length);
		} else {
			ARStartDMA(low->type, low->dest, low->source, low->length);
		}

		__ARQCallbackLo = __ARQRequestPendingLo->callback;
	} else if (low->type == 0) {
		ARStartDMA(low->type, low->source, low->dest, __ARQChunkSize);
	} else {
		ARStartDMA(low->type, low->dest, low->source, __ARQChunkSize);
	}

	__ARQRequestPendingLo->length -= __ARQChunkSize;
	__ARQRequestPendingLo->source += __ARQChunkSize;
	__ARQRequestPendingLo->dest += __ARQChunkSize;
}

void __ARQInterruptServiceRoutine()
{
	if (__ARQCallbackHi) {
		(*__ARQCallbackHi)((u32)__ARQRequestPendingHi);
		__ARQRequestPendingHi = nullptr;
		__ARQCallbackHi       = nullptr;

	} else if (__ARQCallbackLo) {
		(*__ARQCallbackLo)((u32)__ARQRequestPendingLo);
		__ARQRequestPendingLo = nullptr;
		__ARQCallbackLo       = nullptr;
	}

	if (__ARQRequestQueueHi) {
		if (__ARQRequestQueueHi->type == 0) {
			ARStartDMA(__ARQRequestQueueHi->type, __ARQRequestQueueHi->source, __ARQRequestQueueHi->dest, __ARQRequestQueueHi->length);
		} else {
			ARStartDMA(__ARQRequestQueueHi->type, __ARQRequestQueueHi->dest, __ARQRequestQueueHi->source, __ARQRequestQueueHi->length);
		}

		__ARQCallbackHi       = __ARQRequestQueueHi->callback;
		__ARQRequestPendingHi = __ARQRequestQueueHi;
		__ARQRequestQueueHi   = __ARQRequestPendingHi->next;
	}

	if (__ARQRequestPendingHi == nullptr) {
		__ARQPopTaskQueueHi();
	}
}

void ARQInit()
{
	BOOL level;

	if (__ARQ_init_flag == TRUE) {
		return;
	}

	__ARQRequestQueueLo = nullptr;
	__ARQRequestQueueHi = nullptr;
	__ARQChunkSize      = 0x1000;

	level = OSDisableInterrupts();

	__AR_Callback = &__ARQInterruptServiceRoutine;

	(void)OSRestoreInterrupts(level);

	__ARQRequestPendingHi = nullptr;
	__ARQRequestPendingLo = nullptr;
	__ARQCallbackHi       = nullptr;
	__ARQCallbackLo       = nullptr;

	__ARQ_init_flag = TRUE;
}
