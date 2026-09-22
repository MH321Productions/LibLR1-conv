#ifndef LIBLR1_CONV_BINARYREADER_HPP
#define LIBLR1_CONV_BINARYREADER_HPP

#include <cinttypes>
#include <vector>
#include <filesystem>
#include <string>
#include <cstring>
#include <set>

#include <LR1/io/token.hpp>

namespace LR1 {
    class BinaryReader {
        public:
            static constexpr size_t npos = -1;

            BinaryReader() : offset(0) {}
            explicit BinaryReader(const std::vector<uint8_t>& data) : data(data), offset(0) {}
            explicit BinaryReader(const std::filesystem::path& path);

            [[nodiscard]] size_t position() const { return offset; }
            [[nodiscard]] size_t size() const { return data.size(); }

            //Read simple primitives
            uint8_t readByte() {return readNumber<uint8_t>();}
            int8_t readChar() {return readNumber<int8_t>();}
            uint16_t readUShort() {return readNumber<uint16_t>();}
            int16_t readShort() {return readNumber<int16_t>();}
            uint32_t readUInt() {return readNumber<uint32_t>();}
            int32_t readInt() {return readNumber<int32_t>();}
            uint64_t readULong() {return readNumber<uint64_t>();}
            int64_t readLong() {return readNumber<int64_t>();}
            float readFloat() {return readNumber<float>();}
            double readDouble() {return readNumber<double>();}

            uint8_t readByteWithHeader() {return readNumberWithHeader<uint8_t, Token::Byte>();}
            int8_t readCharWithHeader() {return readNumberWithHeader<int8_t, Token::SByte>();}
            uint16_t readUShortWithHeader() {return readNumberWithHeader<uint16_t, Token::UShort>();}
            int16_t readShortWithHeader() {return readNumberWithHeader<int16_t, Token::Short>();}
            int32_t readIntWithHeader() {return readNumberWithHeader<int32_t, Token::Int32>();}
            float readFloatWithHeader() {return readNumberWithHeader<float, Token::Float>();}
            int32_t readIntegralWithHeader();

            //Read arbitrary data buffer
            template<typename TData> std::vector<TData> readBuffer(const size_t& len) {
                std::vector<TData> result(len / sizeof(TData));
                memcpy(result.data(), data.data() + offset, len);
                offset += len;
                return result;
            }
            std::vector<uint8_t> readBytes(const size_t& len) {return readBuffer<uint8_t>(len);}

            Token readToken();
            Token expectToken(Token expected);
            Token expectToken(const std::set<Token>& expected);

            //Read text
            /**
             * Read a 16-Bit Latin1/ISO 8559-1 String (used in SRF files)
             * and convert it to UTF-8
             * @return The UTF-8 converted String
             */
            std::string readWideString();
            std::string readAsciiString(const size_t& numBytes = -1);

        private:
            std::vector<uint8_t> data;
            size_t offset;

            template<typename TNum> TNum readNumber() {
                TNum result;
                memcpy(&result, data.data() + offset, sizeof(TNum));
                offset += sizeof(TNum);
                return result;
            }

            template<typename TNum, Token token> TNum readNumberWithHeader() {
                expectToken(token);
                return readNumber<TNum>();
            }

            /**
             * Convert the given Unicode codepoint to UTF-8
             * @param ch The Unicode codepoint
             * @return The UTF-8 Bytes
             * @see https://de.wikipedia.org/wiki/UTF-8#Algorithmus
             */
            static std::vector<char> getUtf8Bytes(const uint16_t& ch);
    };
}


#endif //LIBLR1_CONV_BINARYREADER_HPP
