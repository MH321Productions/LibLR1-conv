#ifndef LIBLR1_CONV_DECODER_HPP
#define LIBLR1_CONV_DECODER_HPP

#include <optional>
#include <filesystem>
#include <vector>

#include <LR1/jam/extractor.hpp>
#include <LR1/io/binaryfilehelper.hpp>

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

    template<ResourceType restype> class Converter {
    public:
        static ResourceType getType() {return restype;}

        virtual bool convert(const std::filesystem::path& in, const std::filesystem::path& out) = 0;
    };

    template<ResourceType restype, typename TDecoded> class Decoder : public virtual Converter<restype> {
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

    template<ResourceType restype, typename TDecoded> class CompressedBinaryDecoder : public virtual Decoder<restype, TDecoded> {
        public:
            ~CompressedBinaryDecoder() override = default;

            std::optional<TDecoded> decode(const std::filesystem::path& path) override {
                BinaryReader rawReader(path);
                reader = BinaryFileHelper::decompress(rawReader);
                return decode();
            }

            std::optional<TDecoded> decode(const std::vector<uint8_t>& data) override {
                BinaryReader rawReader(data);
                reader = BinaryFileHelper::decompress(rawReader);
                return decode();
            }

        protected:
            BinaryReader reader;

            virtual std::optional<TDecoded> decode() = 0;
    };

}

#endif //LIBLR1_CONV_DECODER_HPP
