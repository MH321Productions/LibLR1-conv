#ifndef LIBLR1_CONV_WRITER_HPP
#define LIBLR1_CONV_WRITER_HPP

#include <vector>
#include <cinttypes>

#include <LR1/io/token.hpp>

namespace LR1 {
    class BinaryWriter {
        public:
            BinaryWriter() : offset(0) {}

            [[nodiscard]] size_t size() const {return data.size();}
            [[nodiscard]] std::vector<uint8_t> buffer() const {return data;}

            //Write simple primitives
            void writeByte(const uint8_t& value) {writeNumber(value);}
            void writeChar(const int8_t& value) {writeNumber(value);}
            void writeUShort(const uint16_t& value) {writeNumber(value);}
            void writeShort(const int16_t& value) {writeNumber(value);}
            void writeUInt(const uint32_t& value) {writeNumber(value);}
            void writeInt(const int32_t& value) {writeNumber(value);}
            void writeFloat(const float& value) {writeNumber(value);}

            void writeByteWithHeader(const uint8_t& value) {writeNumberWithHeader<Token::Byte>(value);}
            void writeCharWithHeader(const int8_t& value) {writeNumberWithHeader<Token::SByte>(value);}
            void writeUShortWithHeader(const uint16_t& value) {writeNumberWithHeader<Token::UShort>(value);}
            void writeShortWithHeader(const int16_t& value) {writeNumberWithHeader<Token::Short>(value);}
            void writeIntWithHeader(const int32_t& value) {writeNumberWithHeader<Token::Int32>(value);}
            void writeFloatWithHeader(const float& value) {writeNumberWithHeader<Token::Float>(value);}

            //Write arbitrary data buffer
            template<typename TData> void writeBuffer(const std::vector<TData>& buffer) {writeBuffer(buffer.data(), buffer.size());}
            template<typename TData> void writeBuffer(const TData* buffer, const size_t& len) {
                const size_t lenBytes = len * sizeof(TData);
                data.resize(data.size() + lenBytes);
                memcpy(data.data() + offset, buffer, lenBytes);
                offset += lenBytes;
            }
            void writeBytes(const std::vector<uint8_t>& buffer) {writeBuffer(buffer.data(), buffer.size());}

            void writeToken(Token token);

            void writeString(const std::string& str);
            void writeStringWithHeader(const std::string& str);

        private:
            size_t offset;
            std::vector<uint8_t> data;

            template<typename TNum> void writeNumber(const TNum& value) {
                data.resize(data.size() + sizeof(TNum));
                memcpy(data.data() + offset, &value, sizeof(TNum));
                offset += sizeof(TNum);
            }

            template<Token token, typename TNum> void writeNumberWithHeader(const TNum& value) {
                writeToken(token);
                return writeNumber(value);
            }
    };
}

#endif //LIBLR1_CONV_WRITER_HPP
