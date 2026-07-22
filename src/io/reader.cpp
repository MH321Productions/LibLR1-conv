#include <fstream>

#include <LR1/io/reader.hpp>

using namespace std;
using namespace std::filesystem;

constexpr uint8_t startByteTemplate2 = 0b110'000'00;
constexpr uint8_t startByteTemplate3 = 0b1110'0000;
constexpr uint8_t followByteTemplate = 0b10'000000;
constexpr char a0a5 = 0b00'111111;
constexpr uint16_t a6b2 = 0b00000'11111'000000;
constexpr uint8_t a6b3 = 0b0000'111111'000000;

namespace LR1 {
    BinaryReader::BinaryReader(const path &path) : data(file_size(path)), offset(0) {
        ifstream file(path, std::ios::binary);
        if (!file) throw runtime_error("Cannot open file " + path.string());

        file.read(reinterpret_cast<char*>(data.data()), static_cast<streamsize>(file_size(path)));
        file.close();
    }

    std::string BinaryReader::readWideString(const size_t& off) {
        string s;
        const size_t startOff = off == npos ? offset : off;
        size_t i;
        for (i = 0; ; i += 2) {
            const uint16_t ch = readUShort(startOff + i);
            if (ch == 0) break;

            for (const char& c : getUtf8Bytes(ch)) s.push_back(c);
        }

        if (off == npos) offset += i;

        return s;
    }

    std::string BinaryReader::readAsciiString(const size_t& numBytes, const size_t& off) {
        string s;
        s.reserve(numBytes);
        const size_t startOff = off == npos ? offset : off;
        for (size_t i = 0; i < numBytes; i++) {
            const char ch = readChar(startOff + i);
            if (ch == 0) break;
            s.push_back(ch);
        }

        if (off == npos) offset += numBytes;

        return s;
    }

    std::vector<char> BinaryReader::getUtf8Bytes(const uint16_t& ch) {
        if (ch < 0x80) return {static_cast<char>(ch)};
        if (ch < 0x0800) return {
            static_cast<char>(startByteTemplate2 | ((ch & a6b2) >> 6)),
            static_cast<char>(followByteTemplate | (ch & a0a5))
        };
        return {
            static_cast<char>(startByteTemplate3 | (ch >> 12)),
            static_cast<char>(followByteTemplate | ((ch & a6b3) >> 6)),
            static_cast<char>(followByteTemplate | (ch & a0a5))
        };
    }
}
