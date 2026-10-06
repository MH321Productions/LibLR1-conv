#include <LR1/io/reader.hpp>
#include <LR1/utils/color.hpp>

namespace LR1 {
    Color Color::read(BinaryReader &reader) {
        Color res{};
        res.r = static_cast<uint8_t>(reader.readIntegralWithHeader());
        res.g = static_cast<uint8_t>(reader.readIntegralWithHeader());
        res.b = static_cast<uint8_t>(reader.readIntegralWithHeader());
        res.a = static_cast<uint8_t>(reader.readIntegralWithHeader());

        return res;
    }

    Color Color::readNoAlpha(BinaryReader &reader) {
        Color res{};
        res.r = static_cast<uint8_t>(reader.readIntegralWithHeader());
        res.g = static_cast<uint8_t>(reader.readIntegralWithHeader());
        res.b = static_cast<uint8_t>(reader.readIntegralWithHeader());

        return res;
    }
}
