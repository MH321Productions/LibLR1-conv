/*
 * This is a C++ port from Will Kirkby's LibLR1 BMP Decoder, which can be found here: https://bitbucket.org/WillKirkby/liblr1/src/master/
 * in "LibLR1/BMP.cs"
 */

#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <iostream>
#include <fstream>
#include <vector>

#include <stb/stb_image_write.h>

#include <LR1/formats/bmp.hpp>

using namespace std;
using namespace std::filesystem;

namespace LR1 {
    enum BmpEncoding : uint8_t {
        Palette4Bit = 0x04,
        Palette8Bit = 0x08,
        RGB = 0x98
    };

    optional<Bmp> BmpDecoder::decode() {
        // Determine Encoding
        const uint8_t encodingByte = reader.readByte();
        BmpEncoding encoding;
        switch (encodingByte) {
            case Palette4Bit:
                encoding = Palette4Bit;
                break;
            case Palette8Bit:
                encoding = Palette8Bit;
                break;
            case RGB:
                encoding = RGB;
                break;
            default:
                cerr << "Unknown BMP encoding: 0x" << hex << static_cast<int>(encodingByte) << endl;
                return nullopt;
        }

        // Determine Palette size
        const uint8_t paletteSize = reader.readByte();

        //Determine image size
        const int16_t width = reader.readShort();
        const int16_t height = reader.readShort();

        //Read palette colors (if available)
        vector<Color> palette;
        if (encoding != RGB) {
            const size_t paletteLength = paletteSize + 1;
            palette.resize(paletteLength);
            for (int i = 0; i < paletteLength; ++i) {
                const uint8_t b = reader.readByte();
                const uint8_t g = reader.readByte();
                const uint8_t r = reader.readByte();
                palette[i] = Color(r, g, b);
            }
        }

        //Determine buffer length
        size_t bufferLength = 0;
        switch (encoding) {
            case Palette4Bit:
                bufferLength = (width * height + 1) / 2;
                break;
            case Palette8Bit:
                bufferLength = width * height;
                break;
            case RGB:
                bufferLength = width * height * 3;
                break;
        }

        //Allocate buffer and read
        auto* buffer = new uint8_t[bufferLength];
        size_t bufferPos = 0;
        while (bufferPos < bufferLength) {
            vector<uint8_t> block = readBlock();
            memcpy(buffer + bufferPos, block.data(), block.size());
            bufferPos += block.size();
        }

        //Convert to RGB
        auto* pixels = new Color[width * height];
        switch (encoding) {
            case Palette4Bit:
                for (size_t i = 0; i < width * height; ++i) {
                    uint8_t index = buffer[i / 2];
                    index >>= 4 * (1 - (i % 2));
                    index &= 0xF;
                    pixels[i] = palette[index];
                }
                break;

            case Palette8Bit:
                for (size_t i = 0; i < width * height; ++i) {
                    const uint8_t index = buffer[i];
                    pixels[i] = palette[index];
                }
                break;

            case RGB:
                //TODO: Check if rgb or bgr is used
                for (size_t i = 0; i < width * height; ++i) {
                    pixels[i] = Color(
                        buffer[i * 3 + 0],
                        buffer[i * 3 + 1],
                        buffer[i * 3 + 2]
                    );
                }
        }


        return Bmp(width, height, pixels);
    }

    bool BmpDecoder::save(const path& path, const Bmp& decoded) {
        return stbi_write_png(path.string().c_str(), decoded.width, decoded.height, 3, decoded.pixels, decoded.width * 3);
    }

    vector<uint8_t> BmpDecoder::readBlock() {
        const int16_t lengthDecompressed = reader.readShort();
        const int16_t lengthCompressed = reader.readShort();

        if (lengthDecompressed == lengthCompressed) return reader.readBuffer<uint8_t>(lengthDecompressed);

        vector<uint8_t> buffer;
        buffer.reserve(lengthDecompressed);

        buffer.push_back(reader.readByte()); //Always read first byte
        while (readSubBlock(buffer)) {}

        if (buffer.size() != lengthDecompressed) cerr << "Wrong buffer size (expected " << lengthDecompressed << ", found " << buffer.size() << ")" << endl;
        return buffer;
    }

    bool BmpDecoder::readSubBlock(vector<uint8_t>& buffer) {
        uint8_t blockMap = reader.readByte();

        for (int i = 0; i < 8; i++) { //For all 8 Bits in blockMap
            if ((blockMap & 0x80) != 0) { //if highest bit is set - RLE
                const uint8_t temp = reader.readByte();
                int repeat = temp & 0x0F;
                int goback = (temp & 0xF0) << 4;

                goback += reader.readByte();

                if (repeat != 0) {
                    repeat = -(repeat - 0x12);
                } else {
                    if (goback == 0) {
                        return false; //Section is finished
                    }

                    repeat = reader.readByte() + 0x12;
                }

                for (int j = 0; j < repeat; j++) {
                    buffer.push_back(buffer.at(buffer.size() - goback));
                }

            } else { //if highest bit is not set - Copy byte
                buffer.push_back(reader.readByte());
            }
            blockMap <<= 1;
        }
        return true;
    }
}