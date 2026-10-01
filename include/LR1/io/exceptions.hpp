#ifndef LIBLR1_CONV_EXCEPTIONS_HPP
#define LIBLR1_CONV_EXCEPTIONS_HPP

#include <format>

namespace LR1 {
    enum class Token : uint8_t;

    class UnexpectedTypeException : public std::runtime_error {
        public:
            UnexpectedTypeException(Token type, const size_t& offset) :
                std::runtime_error(std::format("Unexpected type 0x{:x} at 0x{:x}", static_cast<int>(type), offset)),
                type(type), offset(offset) {}

            const Token type;
            const size_t offset;
    };

    class UnexpectedBlockException : public std::runtime_error {
    public:
        UnexpectedBlockException(const uint8_t& type, const size_t& offset) :
            std::runtime_error(std::format("Unexpected block 0x{:x} at 0x{:x}", static_cast<int>(type), offset)),
            type(type), offset(offset) {}

        const uint8_t type;
        const size_t offset;
    };

    class UnexpectedPropertyException : public std::runtime_error {
        public:
            UnexpectedPropertyException(const uint8_t& propId, const size_t& offset) :
                std::runtime_error(std::format("Unexpected property 0x{:x} at 0x{:x}", propId, offset)),
                propId(propId), offset(offset) {}

            const uint8_t propId;
            const size_t offset;
    };
}

#endif //LIBLR1_CONV_EXCEPTIONS_HPP
