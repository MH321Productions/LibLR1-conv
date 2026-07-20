#ifndef LIBLR1_CONV_BINARYREADER_HPP
#define LIBLR1_CONV_BINARYREADER_HPP

#include <cinttypes>
#include <vector>
#include <filesystem>
#include <string>

namespace LR1 {
    class BinaryReader {
        public:
            static constexpr size_t npos = -1;

            BinaryReader() : offset(0) {}
            explicit BinaryReader(const std::vector<uint8_t>& data) : data(data), offset(0) {}
            explicit BinaryReader(const std::filesystem::path& path);

            //Read simple primitives
            uint8_t readByte(const size_t& off = npos) {return readNumber<uint8_t>(off);}
            int8_t readChar(const size_t& off = npos) {return readNumber<int8_t>(off);}
            uint16_t readUShort(const size_t& off = npos) {return readNumber<uint16_t>(off);}
            int16_t readShort(const size_t& off = npos) {return readNumber<int16_t>(off);}
            uint32_t readUInt(const size_t& off = npos) {return readNumber<uint32_t>(off);}
            int32_t readInt(const size_t& off = npos) {return readNumber<int32_t>(off);}
            uint64_t readULong(const size_t& off = npos) {return readNumber<uint64_t>(off);}
            int64_t readLong(const size_t& off = npos) {return readNumber<int64_t>(off);}
            float readFloat(const size_t& off = npos) {return readNumber<float>(off);}
            double readDouble(const size_t& off = npos) {return readNumber<double>(off);}

            //Read arbitrary data buffer
            template<typename TData> std::vector<TData> readBuffer(const size_t& len, const size_t& off = npos) {
                std::vector<TData> result(len / sizeof(TData));
                memcpy(result.data(), data.data() + (off == npos ? offset : off), len);
                if (off == npos) offset += len;
                return result;
            }

            //Read text
            /**
             * Read a 16-Bit Latin1/ISO 8559-1 String (used in SRF files)
             * and convert it to UTF-8
             * @return The UTF-8 converted String
             */
            std::string readNullTerminatedWideString();
            std::string readAsciiString(const size_t& numBytes, const size_t& off = npos);

        private:
            std::vector<uint8_t> data;
            size_t offset;

            template<typename TNum> TNum readNumber(const size_t& off = npos) {
                size_t realOffset;
                if (off != npos) {
                    realOffset = off;
                } else {
                    realOffset = offset;
                    offset += sizeof(TNum);
                }

                TNum result;
                memcpy(&result, data.data() + realOffset, sizeof(TNum));
                return result;
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
