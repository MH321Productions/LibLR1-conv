#ifndef LIBLR1_CONV_TOKEN_HPP
#define LIBLR1_CONV_TOKEN_HPP

#include <cinttypes>

namespace LR1 {
    enum class Token : uint8_t {
        String = 0x02,
        Float = 0x03,
        Int32 = 0x04,
        LeftCurly = 0x05,
        RightCurly = 0x06,
        LeftBracket = 0x07,
        RightBracket = 0x08,
        Fract8 = 0x0B,
        SByte = 0x0B,
        Byte = 0x0C,
        Fract16 = 0x0D,
        Short = 0x0D,
        UShort = 0x0E,
        QuantizedFloat4096 = 0x0F,
        QuantizedFloat32 = 0x10,
        FloatFromInt16 = 0x11,
        NormalizedFloat127 = 0x12,
        Extended = 0x13,
        Array = 0x14, // compression pass only
        BracketSequence = 0x15, // compression pass only
        Struct = 0x16, // compression pass only
    };
}

#endif //LIBLR1_CONV_TOKEN_HPP
