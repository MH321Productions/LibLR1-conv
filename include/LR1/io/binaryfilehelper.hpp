#ifndef LIBLR1_CONV_BINARYFILEHELPER_HPP
#define LIBLR1_CONV_BINARYFILEHELPER_HPP

#include <map>
#include <cinttypes>
#include <vector>

namespace LR1 {
    class BinaryReader;
    class BinaryWriter;
    enum class Token: uint8_t;

    class BinaryFileHelper {
        public:
            static BinaryReader decompress(BinaryReader& reader, const bool& preserveInlineArrays = false);

        private:
            static void recursiveDecompress(Token blockId, BinaryReader& reader, BinaryWriter& writer, std::map<Token, std::vector<Token>>& structs, const bool& preserveInlineArrays);
            static Token readEncodedToken(BinaryReader& reader);
            static Token readExtendedToken(BinaryReader& reader);
    };
}

#endif //LIBLR1_CONV_BINARYFILEHELPER_HPP
