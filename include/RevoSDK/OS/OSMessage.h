#ifndef _DOLPHIN_OS_OSMESSAGE_H
#define _DOLPHIN_OS_OSMESSAGE_H

#include "RevoSDK/OS/OSThread.h"
#include "RevoSDK/OS/OSUtil.h"
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif // ifdef __cplusplus

///////// MESSAGE TYPES //////////
typedef struct OSMessageQueue OSMessageQueue;

// Useful typedef for messages.
typedef void* OSMessage;

// Struct for managing the message queue.
struct OSMessageQueue {
	OSThreadQueue queueSend;    // _00
	OSThreadQueue queueReceive; // _08
	OSMessage* msgArray;        // _10, array of messages.
	s32 msgCount;               // _14, array limit size.
	s32 firstIndex;             // _18, first message index in array.
	s32 usedCount;              // _1C, actual number of used messages.
};

// Defines for message flags for sending/receiving.
#define OS_MESSAGE_NOBLOCK (0)
#define OS_MESSAGE_BLOCK   (1)

typedef enum {
	OS_MSG_NO_FLAGS = 0,
	OS_MSG_PERSISTENT = (1 << 0),
} OSMessageFlags;

//////////////////////////////////

/////// MESSAGE FUNCTIONS ////////
// Functions for handling messages.
void OSInitMessageQueue(OSMessageQueue* queue, OSMessage* msgArray, s32 msgCount);
BOOL OSSendMessage(OSMessageQueue* queue, OSMessage msg, s32 flags);
BOOL OSJamMessage(OSMessageQueue* queue, OSMessage msg, s32 flags);
BOOL OSReceiveMessage(OSMessageQueue* queue, OSMessage* msgPtr, s32 flags);

//////////////////////////////////

#define OSSendMessageAny(msgQueue_, msg_, flags_) OSSendMessage(msgQueue_, (OSMessage)(msg_), flags_)

#define OSReceiveMessageAny(msgQueue_, msgOut_, flags_) OSReceiveMessage(msgQueue_, (OSMessage*)(msgOut_), flags_)

#define OSJamMessageAny(msgQueue_, msg_, flags_) OSJamMessage(msgQueue_, (OSMessage)(msg_), flags_)

#ifdef __cplusplus
};
#endif // ifdef __cplusplus

#endif
