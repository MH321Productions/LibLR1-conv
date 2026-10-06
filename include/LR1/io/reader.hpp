#ifndef LIBLR1_CONV_BINARYREADER_HPP
#define LIBLR1_CONV_BINARYREADER_HPP

#include <cinttypes>
#include <vector>
#include <filesystem>
#include <string>
#include <cstring>
#include <set>
#include <map>
#include <format>

#include <glm/glm.hpp>

#include <LR1/io/token.hpp>
#include <LR1/io/serializers.hpp>

namespace LR1 {
    class BinaryReader;

    template<typename TSerializer, typename TData>
    concept ISerializer = requires(BinaryReader& reader)
    {
        {TSerializer::read(reader)} -> std::same_as<TData>;
    };

    class BinaryReader {
        public:
            static constexpr size_t npos = -1;

            BinaryReader() : offset(0) {}
            explicit BinaryReader(const std::vector<uint8_t>& data) : data(data), offset(0) {}
            explicit BinaryReader(const std::filesystem::path& path);

            [[nodiscard]] size_t position() const { return offset; }
            [[nodiscard]] size_t size() const { return data.size(); }
            void seek(const size_t& pos) { offset = pos; }

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
            bool next(Token expected);

            //Read text
            /**
             * Read a 16-Bit Latin1/ISO 8559-1 String (used in SRF files)
             * and convert it to UTF-8
             * @return The UTF-8 converted String
             */
            std::string readWideString();
            std::string readString(const size_t& numBytes = -1);
            std::string readStringWithHeader(const size_t& numBytes = -1);

            //Read serializable
            template<typename TData, ISerializer<TData> TSerializer = TData> TData readSerializable() {return TSerializer::read(*this);}
            template<typename TData> TData readSerializable(const std::function<TData(BinaryReader&)>& deserializer) {return deserializer(*this);}

            template<typename TData> std::vector<TData> readArrayBlock(const std::function<TData(BinaryReader&)>& deserializer) {
                expectToken(Token::LeftBracket);
                int arrayLen = readIntWithHeader();
                std::vector<TData> result(arrayLen);
                expectToken(Token::RightBracket);
                expectToken(Token::LeftCurly);
                for (int i = 0; i < arrayLen; i++) result.at(i) = readSerializable(deserializer);
                expectToken(Token::RightCurly);
                return result;
            }
            template<typename TData, ISerializer<TData> TSerializer = TData> std::vector<TData> readArrayBlock() {return readArrayBlock<TData>(&TSerializer::read);}

            template<typename TData> TData readStruct(const std::function<TData(BinaryReader&)>& deserializer) {
                expectToken(Token::LeftCurly);
                TData res = deserializer(*this);
                expectToken(Token::RightCurly);

                return res;
            }
            template<typename TData, ISerializer<TData> TSerializer = TData> TData readStruct() {return readStruct<TData>(&TSerializer::read);}

            template<typename TData> std::map<std::string, TData> readDictionaryBlock(const std::function<TData(BinaryReader&)>& deserializer, const uint8_t& typeByte) {
                std::map<std::string, TData> result;
                expectToken(Token::LeftBracket);
                const int len = readIntWithHeader();
                expectToken(Token::RightBracket);

                expectToken(Token::LeftCurly);
                for (int i = 0; i < len; i++) {
                    expectToken(static_cast<Token>(typeByte));

                    std::string key = std::format("{}", i);
                    if (next(Token::String)) key = readStringWithHeader();

                    TData value = readStruct(deserializer);
                    result.emplace(key, value);
                }
                expectToken(Token::RightCurly);
                return result;
            }
            template<typename TData, ISerializer<TData> TSerializer = TData> std::map<std::string, TData> readDictionaryBlock(const uint8_t& typeByte) {return readDictionaryBlock<TData>(&TSerializer::read, typeByte);}

            std::vector<glm::vec3> readVector3fArrayBlock() {return readArrayBlock<glm::vec3, Serializers::vec3>();}
            std::vector<std::string> readStringArrayBlock() {return readArrayBlock<std::string, Serializers::string>();}

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
