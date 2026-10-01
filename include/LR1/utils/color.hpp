#ifndef LIBLR1_CONV_COLOR_HPP
#define LIBLR1_CONV_COLOR_HPP

#include <cinttypes>

namespace LR1 {
    class BinaryReader;

    struct Color {
        uint8_t r, g, b, a;

        Color() : r(0), g(0), b(0), a(0) {}

        static Color read(BinaryReader& reader);
        static Color readNoAlpha(BinaryReader& reader);
    };
}

#endif //LIBLR1_CONV_COLOR_HPP
