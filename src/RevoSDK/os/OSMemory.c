#include "RevoSDK/hw_regs.h"
#include "RevoSDK/os.h"

// forward declarations.
static BOOL OnShutdown(BOOL final, u32 event);

// Local shutdown function information.
static OSShutdownFunctionInfo ShutdownFunctionInfo = { OnShutdown, OS_RESET_PRIO_MEM };

// useful macros.
#define TRUNC(n, a) (((u32)(n)) & ~((a) - 1))
#define ROUND(n, a) (((u32)(n) + (a) - 1) & ~((a) - 1))


u32 OSGetPhysicalMem1Size()
{
	return *(u32*)(OSPhysicalToCached(0x3100));
}

u32 OSGetPhysicalMem2Size()
{
	return *(u32*)(OSPhysicalToCached(0x3118));
}

u32 OSGetConsoleSimulatedMem1Size()
{
	return *(u32*)(OSPhysicalToCached(0x3104));
}

u32 OSGetConsoleSimulatedMem2Size()
{
	return *(u32*)(OSPhysicalToCached(0x311c));
}
/**
 * @note Address: 0x800EF794
 * @note Size: 0x3C
 */
static BOOL OnShutdown(BOOL final, u32 event)
{
	if (final != FALSE) {
		__MEMRegs[MEM_PROT_TYPE] = 0xFF;
		__OSMaskInterrupts(OS_INTERRUPTMASK_MEM_RESET);
	}
	return TRUE;
}

/**
 * @note Address: 0x800EF7D0
 * @note Size: 0x6C
 */
static void MEMIntrruptHandler(__OSInterrupt interrupt, OSContext* context)
{
	u32 addr;
	u32 cause;

	cause                      = __MEMRegs[MEM_INTRPT_SRC];
	addr                       = (((u32)__MEMRegs[MEM_INTRPT_ADDR_HI] & 0x3ff) << 16) | __MEMRegs[MEM_INTRPT_ADDR_LO];
	__MEMRegs[MEM_INTRPT_FLAG] = 0;

	if (__OSErrorTable[OS_ERROR_PROTECTION]) {
		__OSErrorTable[OS_ERROR_PROTECTION](OS_ERROR_PROTECTION, context, cause, addr);
		return;
	}

	__OSUnhandledException(OS_ERROR_PROTECTION, context, cause, addr);
}

/**
 * @note Address: 0x800EF83C
 * @note Size: 0xC4
 */
void OSProtectRange(u32 channel, void* addr, u32 numBytes, u32 control)
{
	BOOL enabled;
	u32 start;
	u32 end;
	u16 reg;
	if (channel >= 4) {
		return;
	}

	control &= OS_PROTECT_CONTROL_RDWR;

	end   = (u32)addr + numBytes;
	start = TRUNC(addr, 1u << 10);
	end   = ROUND(end, 1u << 10);

	DCFlushRange((void*)start, end - start);

	enabled = OSDisableInterrupts();

	__OSMaskInterrupts(OS_INTERRUPTMASK(__OS_INTERRUPT_MEM_0 + channel));

	__MEMRegs[0 + 2 * channel] = (u16)(start >> 10);
	__MEMRegs[1 + 2 * channel] = (u16)(end >> 10);

	reg = __MEMRegs[MEM_PROT_TYPE];
	reg &= ~(OS_PROTECT_CONTROL_RDWR << 2 * channel);
	reg |= control << 2 * channel;
	__MEMRegs[MEM_PROT_TYPE] = reg;

	if (control != OS_PROTECT_CONTROL_RDWR) {
		__OSUnmaskInterrupts(OS_INTERRUPTMASK(__OS_INTERRUPT_MEM_0 + channel));
	}

	OSRestoreInterrupts(enabled);
}

static asm void ConfigMEM1_24MB(void) {
#ifdef __MWERKS__ // clang-format off
    nofralloc

    li r7, 0
    lis r4, 0x00000002@ha
    addi r4, r4, 0x00000002@l
    lis r3, 0x800001FF@ha
    addi r3, r3, 0x800001FF@l
    lis r6, 0x01000002@ha
    addi r6, r6, 0x01000002@l
    lis r5, 0x810000FF@ha
    addi r5, r5, 0x810000FF@l
    isync
    mtdbatu 0, r7
    mtdbatl 0, r4
    mtdbatu 0, r3
    isync
    mtibatu 0, r7
    mtibatl 0, r4
    mtibatu 0, r3
    isync
    mtdbatu 2, r7
    mtdbatl 2, r6
    mtdbatu 2, r5
    isync
    mtibatu 2, r7
    mtibatl 2, r6
    mtibatu 2, r5
    isync
    mfmsr r3
    ori r3, r3, 0x30
    mtspr 0x1b, r3
    mflr r3
    mtspr 0x1a, r3
    rfi
#endif // clang-format on
}

static asm void ConfigMEM1_48MB(void) {
#ifdef __MWERKS__ // clang-format off
    nofralloc

    li r7, 0
    lis r4, 0x00000002@ha
    addi r4, r4, 0x00000002@l
    lis r3, 0x800003FF@ha
    addi r3, r3, 0x800003FF@l
    lis r6, 0x02000002@ha
    addi r6, r6, 0x02000002@l
    lis r5, 0x820001FF@ha
    addi r5, r5, 0x820001FF@l
    isync
    mtdbatu 0, r7
    mtdbatl 0, r4
    mtdbatu 0, r3
    isync
    mtibatu 0, r7
    mtibatl 0, r4
    mtibatu 0, r3
    isync
    mtdbatu 2, r7
    mtdbatl 2, r6
    mtdbatu 2, r5
    isync
    mtibatu 2, r7
    mtibatl 2, r6
    mtibatu 2, r5
    isync
    mfmsr r3
    ori r3, r3, 0x30
    mtspr 0x1b, r3
    mflr r3
    mtspr 0x1a, r3
    rfi
#endif // clang-format on
}

static asm void ConfigMEM2_52MB(void) {
#ifdef __MWERKS__ // clang-format off
    nofralloc

    li r7, 0
    lis r4, 0x10000002@ha
    addi r4, r4, 0x10000002@l
    lis r3, 0x900003FF@ha
    addi r3, r3, 0x900003FF@l
    lis r6, 0x1000002A@ha
    addi r6, r6, 0x1000002A@l
    lis r5, 0xD00007FF@ha
    addi r5, r5, 0xD00007FF@l
    isync
    mtspr 0x238, r7
    mtspr 0x239, r4
    mtspr 0x238, r3
    isync
    mtspr 0x230, r7
    mtspr 0x231, r4
    mtspr 0x230, r3
    isync
    mtspr 0x23a, r7
    mtspr 0x23b, r6
    mtspr 0x23a, r5
    isync
    mtspr 0x232, r7
    mtspr 0x233, r7
    isync
    lis r4, 0x12000002@ha
    addi r4, r4, 0x12000002@l
    lis r3, 0x920001FF@ha
    addi r3, r3, 0x920001FF@l
    lis r6, 0x13000002@ha
    addi r6, r6, 0x13000002@l
    lis r5, 0x9300007F@ha
    addi r5, r5, 0x9300007F@l
    isync
    mtspr 0x23c, r7
    mtspr 0x23d, r4
    mtspr 0x23c, r3
    isync
    mtspr 0x234, r7
    mtspr 0x235, r4
    mtspr 0x234, r3
    isync
    mtspr 0x23e, r7
    mtspr 0x23f, r6
    mtspr 0x23e, r5
    isync
    mtspr 0x236, r7
    mtspr 0x237, r6
    mtspr 0x236, r5
    isync
    mfmsr r3
    ori r3, r3, 0x30
    mtspr 0x1b, r3
    mflr r3
    mtspr 0x1a, r3
    rfi
#endif // clang-format on
}

static asm void ConfigMEM2_56MB(void) {
#ifdef __MWERKS__ // clang-format off
    nofralloc

    li r7, 0
    lis r4, 0x10000002@ha
    addi r4, r4, 0x10000002@l
    lis r3, 0x900003FF@ha
    addi r3, r3, 0x900003FF@l
    lis r6, 0x1000002A@ha
    addi r6, r6, 0x1000002A@l
    lis r5, 0xD00007FF@ha
    addi r5, r5, 0xD00007FF@l
    isync
    mtspr 0x238, r7
    mtspr 0x239, r4
    mtspr 0x238, r3
    isync
    mtspr 0x230, r7
    mtspr 0x231, r4
    mtspr 0x230, r3
    isync
    mtspr 0x23a, r7
    mtspr 0x23b, r6
    mtspr 0x23a, r5
    isync
    mtspr 0x232, r7
    mtspr 0x233, r7
    isync
    lis r4, 0x12000002@ha
    addi r4, r4, 0x12000002@l
    lis r3, 0x920001FF@ha
    addi r3, r3, 0x920001FF@l
    lis r6, 0x13000002@ha
    addi r6, r6, 0x13000002@l
    lis r5, 0x930000FF@ha
    addi r5, r5, 0x930000FF@l
    isync
    mtspr 0x23c, r7
    mtspr 0x23d, r4
    mtspr 0x23c, r3
    isync
    mtspr 0x234, r7
    mtspr 0x235, r4
    mtspr 0x234, r3
    isync
    mtspr 0x23e, r7
    mtspr 0x23f, r6
    mtspr 0x23e, r5
    isync
    mtspr 0x236, r7
    mtspr 0x237, r6
    mtspr 0x236, r5
    isync
    mfmsr r3
    ori r3, r3, 0x30
    mtspr 0x1b, r3
    mflr r3
    mtspr 0x1a, r3
    rfi
#endif // clang-format on
}

static asm void ConfigMEM2_64MB(void) {
#ifdef __MWERKS__ // clang-format off
    nofralloc

    li r7, 0
    lis r4, 0x10000002@ha
    addi r4, r4, 0x10000002@l
    lis r3, 0x900007FF@ha
    addi r3, r3, 0x900007FF@l
    lis r6, 0x1000002A@ha
    addi r6, r6, 0x1000002A@l
    lis r5, 0xD00007FF@ha
    addi r5, r5, 0xD00007FF@l
    isync
    mtspr 0x238, r7
    mtspr 0x239, r4
    mtspr 0x238, r3
    isync
    mtspr 0x230, r7
    mtspr 0x231, r4
    mtspr 0x230, r3
    isync
    mtspr 0x23a, r7
    mtspr 0x23b, r6
    mtspr 0x23a, r5
    isync
    mtspr 0x232, r7
    mtspr 0x233, r7
    isync
    mtspr 0x234, r7
    mtspr 0x235, r7
    isync
    mtspr 0x236, r7
    mtspr 0x237, r7
    isync
    mtspr 0x23c, r7
    mtspr 0x23d, r7
    isync
    mtspr 0x23e, r7
    mtspr 0x23f, r7
    isync
    mfmsr r3
    ori r3, r3, 0x30
    mtspr 0x1b, r3
    mflr r3
    mtspr 0x1a, r3
    rfi
#endif // clang-format on
}

static asm void ConfigMEM2_112MB(void) {
#ifdef __MWERKS__ // clang-format off
    nofralloc

    li r7, 0
    lis r4, 0x10000002@ha
    addi r4, r4, 0x10000002@l
    lis r3, 0x900007FF@ha
    addi r3, r3, 0x900007FF@l
    lis r6, 0x1000002A@ha
    addi r6, r6, 0x1000002A@l
    lis r5, 0xD0000FFF@ha
    addi r5, r5, 0xD0000FFF@l
    isync
    mtspr 0x238, r7
    mtspr 0x239, r4
    mtspr 0x238, r3
    isync
    mtspr 0x230, r7
    mtspr 0x231, r4
    mtspr 0x230, r3
    isync
    mtspr 0x23a, r7
    mtspr 0x23b, r6
    mtspr 0x23a, r5
    isync
    mtspr 0x232, r7
    mtspr 0x233, r7
    isync
    lis r4, 0x14000002@ha
    addi r4, r4, 0x14000002@l
    lis r3, 0x940003FF@ha
    addi r3, r3, 0x940003FF@l
    lis r6, 0x16000002@ha
    addi r6, r6, 0x16000002@l
    lis r5, 0x960001FF@ha
    addi r5, r5, 0x960001FF@l
    isync
    mtspr 0x23c, r7
    mtspr 0x23d, r4
    mtspr 0x23c, r3
    isync
    mtspr 0x234, r7
    mtspr 0x235, r4
    mtspr 0x234, r3
    isync
    mtspr 0x23e, r7
    mtspr 0x23f, r6
    mtspr 0x23e, r5
    isync
    mtspr 0x236, r7
    mtspr 0x237, r6
    mtspr 0x236, r5
    isync
    mfmsr r3
    ori r3, r3, 0x30
    mtspr 0x1b, r3
    mflr r3
    mtspr 0x1a, r3
    rfi
#endif // clang-format on
}

static asm void ConfigMEM2_128MB(void) {
#ifdef __MWERKS__ // clang-format off
    nofralloc

    li r7, 0
    lis r4, 0x10000002@ha
    addi r4, r4, 0x10000002@l
    lis r3, 0x90000FFF@ha
    addi r3, r3, 0x90000FFF@l
    lis r6, 0x1000002A@ha
    addi r6, r6, 0x1000002A@l
    lis r5, 0xD0000FFF@ha
    addi r5, r5, 0xD0000FFF@l
    isync
    mtspr 0x238, r7
    mtspr 0x239, r4
    mtspr 0x238, r3
    isync
    mtspr 0x230, r7
    mtspr 0x231, r4
    mtspr 0x230, r3
    isync
    mtspr 0x23a, r7
    mtspr 0x23b, r6
    mtspr 0x23a, r5
    isync
    mtspr 0x232, r7
    mtspr 0x233, r7
    isync
    mtspr 0x234, r7
    mtspr 0x235, r7
    isync
    mtspr 0x236, r7
    mtspr 0x237, r7
    isync
    mtspr 0x23c, r7
    mtspr 0x23d, r7
    isync
    mtspr 0x23e, r7
    mtspr 0x23f, r7
    isync
    mfmsr r3
    ori r3, r3, 0x30
    mtspr 0x1b, r3
    mflr r3
    mtspr 0x1a, r3
    rfi
#endif // clang-format on
}

static asm void ConfigMEM_ES1_0(void) {
#ifdef __MWERKS__ // clang-format off
    nofralloc

    li r7, 0
    lis r4, 0x00000002@ha
    addi r4, r4, 0x00000002@l
    lis r3, 0x80000FFF@ha
    addi r3, r3, 0x80000FFF@l
    isync
    mtdbatu 0, r7
    mtdbatl 0, r4
    mtdbatu 0, r3
    isync
    mtibatu 0, r7
    mtibatl 0, r4
    mtibatu 0, r3
    isync
    mfmsr r3
    ori r3, r3, 0x30
    mtspr 0x1b, r3
    mflr r3
    mtspr 0x1a, r3
    rfi
#endif // clang-format on
}


/**
 * @note Address: 0x800EFA00
 * @note Size: 0x18
 */
ASM static void RealMode(register u32 addr)
{
#ifdef __MWERKS__ // clang-format off
	nofralloc
	clrlwi r3, r3, 2
	mtsrr0 r3
	mfmsr r3
	rlwinm r3, r3, 0, 28, 25
	mtsrr1 r3
	rfi
#endif // clang-format on
}

static void BATConfig(void) {
    u32 size1, size2, prot;

    if (*(u32*)OSPhysicalToCached(0x3138) == 0) {
        if (OSGetPhysicalMem1Size == 0) {
            RealMode((u32)ConfigMEM_ES1_0);
            return;
        }
    }

    size1 = OSGetConsoleSimulatedMem1Size();

    if (size1 < OSGetPhysicalMem1Size() && size1 == 25165824) {
        DCInvalidateRange((void*)0x81800000, 25165824);
        __MEMRegs[0x14] = 2;
    }

    if (size1 <= 25165824) {
        RealMode((u32)ConfigMEM1_24MB);
    }
    else if (size1 <= 50331648) {
        RealMode((u32)ConfigMEM1_48MB);
    }

    size2 = OSGetConsoleSimulatedMem2Size();
    prot = *(u32*)OSPhysicalToCached(0x3120);

    if (size2 <= 67108864) {
        if (prot <= 0x93400000) {
            RealMode((u32)ConfigMEM2_52MB);
        }
        else if (prot <= 0x93800000) {
            RealMode((u32)ConfigMEM2_56MB);
        }
        else {
            RealMode((u32)ConfigMEM2_64MB);
        }
    }
    else if (size2 <= 134217728) {
        if (prot <= 0x97000000) {
            RealMode((u32)ConfigMEM2_112MB);
        }
        else {
            RealMode((u32)ConfigMEM2_128MB);
        }
    }
}

/**
 * @note Address: 0x800EFA18
 * @note Size: 0x118
 */
void __OSInitMemoryProtection()
{
	BOOL enabled       = OSDisableInterrupts();
	

	__MEMRegs[MEM_INTRPT_FLAG] = 0;
	__MEMRegs[MEM_PROT_TYPE]   = 0xFF;

	__OSMaskInterrupts(OS_INTERRUPTMASK_MEM_0 | OS_INTERRUPTMASK_MEM_1 | OS_INTERRUPTMASK_MEM_2 | OS_INTERRUPTMASK_MEM_3);
	__OSSetInterruptHandler(__OS_INTERRUPT_MEM_0, MEMIntrruptHandler);
	__OSSetInterruptHandler(__OS_INTERRUPT_MEM_1, MEMIntrruptHandler);
	__OSSetInterruptHandler(__OS_INTERRUPT_MEM_2, MEMIntrruptHandler);
	__OSSetInterruptHandler(__OS_INTERRUPT_MEM_3, MEMIntrruptHandler);
	__OSSetInterruptHandler(__OS_INTERRUPT_MEM_ADDRESS, MEMIntrruptHandler);
	OSRegisterShutdownFunction(&ShutdownFunctionInfo);
	
	BATConfig();

	/*if (OSGetConsoleSimulatedMemSize() < OSGetPhysicalMemSize() && OSGetConsoleSimulatedMemSize() == 0x1800000) {
		DCInvalidateRange((void*)0x81800000, 0x1800000);
		__MEMRegs[MEM_UNK_FLAG] = 2;
	}

	if (simulatedSize <= 0x1800000) {
		RealMode((u32)&Config24MB);
	} else if (simulatedSize <= 0x3000000) {
		RealMode((u32)&Config48MB);
	}*/

	__OSUnmaskInterrupts(OS_INTERRUPTMASK_MEM_ADDRESS);
	OSRestoreInterrupts(enabled);
}
