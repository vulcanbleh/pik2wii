#ifndef _REVOSDK_OS_OSRESET_SW_H
#define _REVOSDK_OS_OSRESET_SW_H

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*OSResetCallback)(void);
typedef void (*OSPowerCallback)(void);

BOOL OSGetResetButtonState(void);

OSResetCallback OSSetResetCallback(OSResetCallback);
OSPowerCallback OSSetPowerCallback(OSPowerCallback);

#ifdef __cplusplus
}
#endif

#endif
