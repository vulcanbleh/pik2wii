#ifndef _REVOSDK_OS_OSEXECPARAMS_H
#define _REVOSDK_OS_OSEXECPARAMS_H

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif // ifdef __cplusplus

typedef struct {
    BOOL valid;
    u32 restartCode;
    u32 bootDol;
    void* regionStart;
    void* regionEnd;
    BOOL argsUseDefault;
    void* argsAddr;
} OSExecParams;

void __OSGetExecParams(OSExecParams *);

//////////////////////////////////

#ifdef __cplusplus
};
#endif // ifdef __cplusplus

#endif
