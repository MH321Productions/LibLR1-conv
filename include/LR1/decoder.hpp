#ifndef LIBLR1_CONV_DECODER_HPP
#define LIBLR1_CONV_DECODER_HPP

#include <cinttypes>
#include <optional>
#include <filesystem>
#include <vector>

#include "jam/JamExtractor.hpp"

namespace LR1 {
    struct FileWrapper {
        FileWrapper(const std::filesystem::path& filePath, const ResourceType type) : realPath(filePath), jamFile("", {}), type(type) {}
        FileWrapper(const JamFile& jamFile): jamFile(jamFile), type(jamFile.type) {}

        std::filesystem::path realPath;
        JamFile jamFile;
        ResourceType type;

        [[nodiscard]] bool isRealFile() const {return jamFile.filename.empty();}
        [[nodiscard]] bool isJamFile() const {return !jamFile.filename.empty();}
        [[nodiscard]] std::string filename() const {return isRealFile() ? realPath.filename().string() : jamFile.filename;}
        [[nodiscard]] std::string stem() const {return isRealFile() ? realPath.stem().string() : jamFile.stem();}
        [[nodiscard]] std::string extension() const {return isRealFile() ? realPath.extension().string() : jamFile.extension();}
    };

    template<ResourceType restype, typename TDecoded> class Decoder {
        public:
            virtual ~Decoder() = default;

            static ResourceType getType() {return restype;}

            virtual std::optional<TDecoded> decode(const std::filesystem::path& path) = 0;
            virtual std::optional<TDecoded> decode(const std::vector<uint8_t>& data) = 0;
            std::optional<TDecoded> decode(const FileWrapper& wrapper) {return wrapper.isRealFile() ? decode(wrapper.realPath) : decode(wrapper.jamFile.data);}

            virtual bool save(const std::filesystem::path& path, const TDecoded& decoded) = 0;
    };

    template<ResourceType restype, typename TDecoded> class SimpleBinaryDecoder : public virtual Decoder<restype, TDecoded> {
        public:
            ~SimpleBinaryDecoder() override = default;

            std::optional<TDecoded> decode(const std::filesystem::path& path) override {
                reader = BinaryReader(path);
                return decode();
            }

            std::optional<TDecoded> decode(const std::vector<uint8_t>& data) override {
                reader = BinaryReader(data);
                return decode();
            }

        protected:
            BinaryReader reader;

            virtual std::optional<TDecoded> decode() = 0;
    };

}

#endif //LIBLR1_CONV_DECODER_HPP
