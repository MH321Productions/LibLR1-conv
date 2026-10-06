#include <fstream>
#include <format>

#include <LR1/io/reader.hpp>
#include <LR1/io/exceptions.hpp>

using namespace std;
using namespace std::filesystem;

constexpr uint8_t startByteTemplate2 = 0b110'000'00;
constexpr uint8_t startByteTemplate3 = 0b1110'0000;
constexpr uint8_t followByteTemplate = 0b10'000000;
constexpr char a0a5 = 0b00'111111;
constexpr uint16_t a6b2 = 0b00000'11111'000000;
constexpr uint16_t a6b3 = 0b0000'111111'000000;

namespace LR1 {
    BinaryReader::BinaryReader(const path &path) : data(file_size(path)), offset(0) {
        ifstream file(path, std::ios::binary);
        if (!file) throw runtime_error("Cannot open file " + path.string());

        file.read(reinterpret_cast<char*>(data.data()), static_cast<streamsize>(file_size(path)));
        file.close();
    }

    std::string BinaryReader::readWideString() {
        string s;
        while (true) {
            const uint16_t ch = readUShort();
            if (ch == 0) break;

            for (const char& c : getUtf8Bytes(ch)) s.push_back(c);
        }

        return s;
    }

    std::string BinaryReader::readString(const size_t& numBytes) {
        const size_t currentOffset = offset;
        string s;
        for (size_t i = 0; i < numBytes; i++) {
            const char ch = readChar();
            if (ch == 0) break;
            s.push_back(ch);
        }

        if (numBytes != npos) seek(currentOffset + numBytes);

        return s;
    }

    std::string BinaryReader::readStringWithHeader(const size_t& numBytes) {
        expectToken(Token::String);
        return readString(numBytes);
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

    int32_t BinaryReader::readIntegralWithHeader() {
        switch (const Token type = expectToken({Token::SByte, Token::Byte, Token::Int32, Token::UShort, Token::Short})) {
            case Token::SByte: return readChar();
            case Token::Byte: return readByte();
            case Token::Int32: return readInt();
            case Token::UShort: return readUShort();
            case Token::Short: return readShort();
            default: throw UnexpectedTypeException(type, offset);
        }
    }

    Token BinaryReader::readToken() {
        return static_cast<Token>(readByte());
    }

    Token BinaryReader::expectToken(Token expected) {
        Token actual = readToken();
        if (actual != expected) throw runtime_error(format("Invalid data. Expected 0x{:x}, got 0x{:x}", static_cast<int>(expected), static_cast<int>(actual)));
        return actual;
    }

    Token BinaryReader::expectToken(const set<Token>& expected) {
        Token actual = readToken();
        if (!expected.contains(actual)) {
            string list = "[";
            for (const Token& token : expected) list.append(format("0x{:x}", static_cast<int>(token)));
            list.pop_back();
            list.pop_back();
            list.push_back(']');

            throw runtime_error(format("Invalid data. Expected {}, got 0x{:x}", list, static_cast<int>(actual)));
        }
        return actual;
    }

    bool BinaryReader::next(const Token expected) {
        const Token actual = readToken();
        offset--;
        return actual == expected;
    }
}
