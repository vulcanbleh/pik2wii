#ifndef _REVOSDK_OS_STATE_FLAGS_H
#define _REVOSDK_OS_STATE_FLAGS_H

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif // ifdef __cplusplus

typedef struct {
    u32 checkSum;
    u8 lastBootApp;
    u8 lastShutdown;
    u8 lastDiscState;
    u8 menuMode;
    u8 reserved[24];
} OSStateFlags;

BOOL __OSWriteStateFlags(OSStateFlags *);
BOOL __OSReadStateFlags(OSStateFlags *);

#ifdef __cplusplus
};
#endif // ifdef __cplusplus

#endif
