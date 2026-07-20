#ifndef LIBLR1_CONV_BMP_HPP
#define LIBLR1_CONV_BMP_HPP

#include <LR1/decoder.hpp>
#include <LR1/io/BinaryReader.hpp>

namespace LR1 {
    struct Color {
        uint8_t r, g, b;
    };

    struct Bmp {
        int width, height;
        Color* pixels;
    };

    class BmpDecoder : public virtual Decoder<ResourceType::Image, Bmp> {
        public:
            BmpDecoder() = default;

            std::optional<Bmp> decode(const std::filesystem::path &path) override;
            std::optional<Bmp> decode(const std::vector<uint8_t> &data) override;

            bool save(const std::filesystem::path &path, const Bmp &decoded) override;

        private:
            BinaryReader reader;

            std::optional<Bmp> decode();

            std::vector<uint8_t> readBlock();
            bool readSubBlock(std::vector<uint8_t>& buffer);
    };
}

#endif //LIBLR1_CONV_BMP_HPP
