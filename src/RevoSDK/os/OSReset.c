#include "RevoSDK/hw_regs.h"
#include "RevoSDK/os.h"

static OSShutdownFunctionQueue ShutdownFunctionQueue;
static u32 bootThisDol;
volatile BOOL __OSIsReturnToIdle;

extern BOOL __OSInNandBoot;
extern BOOL __OSInReboot;
/**
 * @note Address: 0x800F02A4
 * @note Size: 0x84
 */
void OSRegisterShutdownFunction(OSShutdownFunctionInfo* info)
{
	OSShutdownFunctionInfo* tmp;
	OSShutdownFunctionInfo* iter;

	for (iter = ShutdownFunctionQueue.head; iter && iter->priority <= info->priority; iter = iter->next) {
		;
	}

	if (iter == nullptr) {
		tmp = ShutdownFunctionQueue.tail;
		if (tmp == nullptr) {
			ShutdownFunctionQueue.head = info;
		} else {
			tmp->next = info;
		}
		info->prev              = tmp;
		info->next              = nullptr;
		ShutdownFunctionQueue.tail = info;
		return;
	}

	info->next = iter;
	tmp        = iter->prev;
	iter->prev = info;
	info->prev = tmp;
	if (tmp == nullptr) {
		ShutdownFunctionQueue.head = info;
		return;
	}
	tmp->next = info;
}

/**
 * @note Address: N/A
 * @note Size: 0x94
 */
BOOL __OSCallShutdownFunctions(u32 pass, u32 event)
{
	OSShutdownFunctionInfo* iter;
	BOOL failure;
    u32 prio;

    prio = 0;
    failure = FALSE;

    for (iter = ShutdownFunctionQueue.head; iter != NULL; iter = iter->next) {
        if (failure && prio != iter->priority) {
            break;
        }

        failure |= !iter->func(pass, event);
        prio = iter->priority;
    }

    failure |= !__OSSyncSram();

    return !failure;
}

static void KillThreads(void);

void __OSShutdownDevices(u32 event) {
    BOOL rc, disableRecalibration, doRecal;

    switch(event) {
      case 0:
      case 5:
      case 6:
        doRecal = FALSE;
        break;
      case 2:
      case 3:
      case 4:
      case 1:
      default:
        doRecal = TRUE;
        break;
    }

    __OSStopAudioSystem();

    if (!doRecal) {
        disableRecalibration = __PADDisableRecalibration(TRUE);
    }

    while (!__OSCallShutdownFunctions(FALSE, event));

    while (!__OSSyncSram());

    OSDisableInterrupts();
    rc = __OSCallShutdownFunctions(TRUE, event);
    ASSERT(rc);
    LCDisable();

    if (!doRecal) {
        __PADDisableRecalibration(disableRecalibration);
    }

    KillThreads();
}

void __OSHotResetForError(void) {
    if (__OSInNandBoot || __OSInReboot) {
        __OSInitSTM();
    }

    __OSHotReset();

    OSPanic(__FILE__, 0x3D3, "__OSHotReset(): Falied to reset system.\n");
}

void OSRestart(u32 resetCode) {
    u8 type = OSGetAppType();
    __OSStopPlayRecord();
    __OSUnRegisterStateEvent();

    if (type == 0x81) {
        OSDisableScheduler();
        __OSShutdownDevices(4);
        OSEnableScheduler();
        __OSRelaunchTitle(resetCode);
    }
    else if (type == 0x80) {
        OSDisableScheduler();
        __OSShutdownDevices(4);
        OSEnableScheduler();
        __OSReboot(resetCode, bootThisDol);
    }

    OSDisableScheduler();
    __OSShutdownDevices(1);
    __OSHotResetForError();
}



void __OSReturnToMenu(u8 menuMode) {
    OSStateFlags state;

    __OSStopPlayRecord();
    __OSUnRegisterStateEvent();
    __DVDPrepareReset();
    __OSReadStateFlags(&state);
    state.lastDiscState = __OSGetDiscState(state.lastDiscState);
    state.lastShutdown = 3;
    state.menuMode = menuMode;
    __OSClearRTCFlags();
    __OSWriteStateFlags(&state);
    OSDisableScheduler();
    __OSShutdownDevices(5);
    OSEnableScheduler();
    __OSLaunchMenu();
    OSDisableScheduler();
    __VISetRGBModeImm();
    __OSHotResetForError();
}


/**
 * @note Address: N/A
 * @note Size: 0x68
 */
static void KillThreads()
{
	OSThread* thread;
	OSThread* next;

	for (thread = __OSActiveThreadQueue.head; thread; thread = next) {
		next = thread->linkActive.next;
		switch (thread->state) {
		case 1:
		case 4:
			OSCancelThread(thread);
			break;
		}
	}
}


/**
 * @note Address: 0x800F03E0
 * @note Size: 0x2BC
 */
void OSResetSystem(int reset, u32 resetCode, BOOL forceMenu)
{
    OSErrorLine(1130,"OSResetSystem() is obsoleted. It doesn't work any longer.\n");
}
