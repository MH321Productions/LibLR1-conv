#include <iostream>

#include <LR1/io/writer.hpp>

using namespace std;

namespace LR1 {
    void BinaryWriter::writeToken(Token token) {
        writeByte(static_cast<uint8_t>(token));
    }

    void BinaryWriter::writeString(const string& str) {
        writeBuffer(str.c_str(), str.size() + 1);
    }

    void BinaryWriter::writeStringWithHeader(const string& str) {
        writeToken(Token::String);
        writeString(str);
    }
}
