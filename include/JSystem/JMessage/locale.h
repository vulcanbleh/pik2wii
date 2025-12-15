#ifndef _JSYSTEM_JMESSAGE_LOCALE_H
#define _JSYSTEM_JMESSAGE_LOCALE_H

#include "types.h"

namespace JMessage {

struct locale {
    typedef int (*parseCharacter_function)(const char**);

    static bool isLeadByte_ShiftJIS(int c) {
        return c >= 0x81 && (c <= 0x9F || (c >= 0xE0 && c <= 0xFC));
    }

    static int parseCharacter_1Byte(const char** ppszText);
    static int parseCharacter_2Byte(const char** ppszText);
    static int parseCharacter_ShiftJIS(const char** ppszText);
    static int parseCharacter_UTF8(const char** ppszText);
};
};  // namespace JMessage

#endif