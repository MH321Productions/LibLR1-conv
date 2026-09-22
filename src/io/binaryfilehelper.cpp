#include <format>
#include <limits>

#include <LR1/io/binaryfilehelper.hpp>
#include <LR1/io/reader.hpp>
#include <LR1/io/writer.hpp>
#include <LR1/io/token.hpp>

using namespace std;

namespace LR1 {
    BinaryReader BinaryFileHelper::decompress(BinaryReader& reader, const bool& preserveInlineArrays) {
        map<Token, vector<Token>> structs;
        BinaryWriter writer;

        while (reader.position() < reader.size()) {
            const Token blockId = readEncodedToken(reader);
            recursiveDecompress(blockId, reader, writer, structs, preserveInlineArrays);
        }

        return BinaryReader(writer.buffer());
    }

    Token BinaryFileHelper::readEncodedToken(BinaryReader& reader) {
        const Token token = reader.readToken();
        return token == Token::Extended ? readExtendedToken(reader) : token;
    }

    Token BinaryFileHelper::readExtendedToken(BinaryReader& reader) {
        const uint16_t token = reader.readShort();
        if (token > numeric_limits<uint8_t>::max()) throw runtime_error(format("Extended binary token 0x{:x} can not be represented by LibLR1's byte token model", token));
        return static_cast<Token>(token);
    }

    void BinaryFileHelper::recursiveDecompress(const Token blockId, BinaryReader& reader, BinaryWriter& writer, map<Token, vector<Token>>& structs, const bool& preserveInlineArrays) {
        uint8_t structLen;
        uint16_t arrayLen;
        Token arrayType, structId;
        vector<Token> structDef;

        switch (blockId) {
            case Token::String:
                writer.writeStringWithHeader(reader.readAsciiString());
                break;

            case Token::Float:
            case Token::Int32:
                writer.writeToken(blockId);
                writer.writeBytes(reader.readBytes(4));
                break;

            case Token::LeftCurly:
            case Token::RightCurly:
            case Token::LeftBracket:
            case Token::RightBracket: //Just copy the token
                writer.writeToken(blockId);
                break;

            case Token::SByte:
            case Token::Byte:
                writer.writeToken(blockId);
                writer.writeBytes(reader.readBytes(1));
                break;

            case Token::Short:
            case Token::UShort:
                writer.writeToken(blockId);
                writer.writeBytes(reader.readBytes(2));
                break;

            case Token::QuantizedFloat4096:
                writer.writeFloatWithHeader(static_cast<float>(reader.readShort()) / 4096.0f);
                break;

            case Token::QuantizedFloat32:
                writer.writeFloatWithHeader(static_cast<float>(reader.readShort()) / 32.0f);
                break;

            case Token::FloatFromInt16:
                writer.writeFloatWithHeader(reader.readShort());
                break;

            case Token::NormalizedFloat127:
                writer.writeFloatWithHeader(static_cast<float>(reader.readByte()) / 127.0f);
                break;

            case Token::Extended:
                recursiveDecompress(readExtendedToken(reader), reader, writer, structs, preserveInlineArrays);
                break;

            case Token::Array:
                arrayLen = reader.readUShort();
                arrayType = readEncodedToken(reader);

                if (preserveInlineArrays) writer.writeToken(Token::LeftBracket);
                for (int i = 0; i < arrayLen; i++) recursiveDecompress(arrayType, reader, writer, structs, preserveInlineArrays);
                if (preserveInlineArrays) writer.writeToken(Token::RightBracket);

                break;

            case Token::BracketSequence:
                recursiveDecompress(Token::LeftBracket, reader, writer, structs, preserveInlineArrays);
                recursiveDecompress(Token::Int32, reader, writer, structs, preserveInlineArrays);
                recursiveDecompress(Token::RightBracket, reader, writer, structs, preserveInlineArrays);
                recursiveDecompress(Token::RightBracket, reader, writer, structs, preserveInlineArrays);
                break;

            case Token::Struct:
                structId = readEncodedToken(reader);
                structLen = reader.readByte();
                structDef.reserve(structLen);
                for (int i = 0; i < structLen; i++) structDef.push_back(readEncodedToken(reader));
                structs.insert(make_pair(structId, structDef));

                break;

            default:
                if (structs.contains(blockId)) { //It's a struct
                    for (int i = 0; i < structs.at(blockId).size(); i++)
                        recursiveDecompress(structs.at(blockId).at(i), reader, writer, structs, preserveInlineArrays);
                } else { //It's a file-specific block token
                    writer.writeToken(blockId);
                }
        }
    }
}
