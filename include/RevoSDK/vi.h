#ifndef _REVOSDK_VI_H
#define _REVOSDK_VI_H

#include "types.h"
#include "RevoSDK/vi/vitypes.h"
#include "RevoSDK/gx/GXTypes.h"

#ifdef __cplusplus
extern "C" {
#endif // ifdef __cplusplus

////////////////////////////////////

///// VIDEO INTERFACE FUNCTIONS ////
// Basic VI functions.
void VIInit(void);
void VIFlush(void);
void VIWaitForRetrace(void);

// Configure functions.
void VIConfigure(const GXRenderModeObj*);
void VIConfigurePan(u16 panPosX, u16 panPosY, u16 panSizeX, u16 panSizeY);

// Retrace callbacks.
VIRetraceCallback VISetPreRetraceCallback(VIRetraceCallback callback);
VIRetraceCallback VISetPostRetraceCallback(VIRetraceCallback callback);

// Getters and setters
void VISetNextFrameBuffer(void* fb);
void* VIGetCurrentFrameBuffer();

void __VIGetCurrentPosition(s16* x, s16* y);

void VISetBlack(BOOL isBlack);

u32 VIGetRetraceCount(void);
u32 VIGetNextField(void);
u32 VIGetCurrentLine(void);
u32 VIGetTvFormat(void);

u32 VIGetDTVStatus(void);

// Unused/stripped in P2.

void* VIGetNextFrameBuffer();
void VISetNextRightFrameBuffer(void* fb);
void VISet3D(); // unsure on arguments

////////////////////////////////////

#ifdef __cplusplus
};
#endif // ifdef __cplusplus

#endif
