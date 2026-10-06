#ifndef LIBLR1_CONV_COLOR_HPP
#define LIBLR1_CONV_COLOR_HPP

#include <cinttypes>

namespace LR1 {
    class BinaryReader;

    struct Color {
        uint8_t r, g, b, a;

        Color() : r(0), g(0), b(0), a(0) {}
        Color(const uint8_t& r, const uint8_t& g, const uint8_t& b, const uint8_t& a) : r(r), g(g), b(b), a(a) {}
        Color(const uint8_t& r, const uint8_t& g, const uint8_t& b) : r(r), g(g), b(b), a(0xFF) {}

        [[nodiscard]] float rF() const {return static_cast<float>(r) / 255.0f;}
        [[nodiscard]] float gF() const {return static_cast<float>(g) / 255.0f;}
        [[nodiscard]] float bF() const {return static_cast<float>(b) / 255.0f;}
        [[nodiscard]] float aF() const {return static_cast<float>(a) / 255.0f;}

        static Color read(BinaryReader& reader);
        static Color readNoAlpha(BinaryReader& reader);
    };
}

#endif //LIBLR1_CONV_COLOR_HPP
