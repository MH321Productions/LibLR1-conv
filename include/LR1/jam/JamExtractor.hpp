#ifndef LIBLR1_CONV_JAMEXTRACTOR_HPP
#define LIBLR1_CONV_JAMEXTRACTOR_HPP

#include <string>
#include <vector>
#include <filesystem>
#include <optional>

#include <LR1/types.hpp>
#include <LR1/io/BinaryReader.hpp>

namespace LR1 {
    struct JamDirectory;

    struct JamFile {
        const std::string filename;
        const std::vector<uint8_t> data;
        ResourceType type;
        JamDirectory* parent = nullptr;

        [[nodiscard]] std::string extension() const;
        [[nodiscard]] std::string stem() const;
    };

    struct JamDirectory {
        JamDirectory(const std::string& dirname, const size_t& offset) : name(dirname), offset(offset), parent(nullptr) {}

        void addChild(JamFile& file);
        void addChild(JamDirectory& dir);

        const std::string name;
        const size_t offset;
        JamDirectory* parent;
        std::vector<JamFile> childFiles;
        std::vector<JamDirectory> childDirectories;
    };


    class JamExtractor {
    public:
        std::optional<JamDirectory> loadJam(const std::filesystem::path& file);

    private:
        BinaryReader buf;

        void recurseChildren(JamDirectory& dir, const size_t& offset);
        [[nodiscard]] std::vector<JamFile> listFiles(const uint32_t& number, const size_t& offset);
        [[nodiscard]] std::vector<JamDirectory> listDirectories(const uint32_t& number, const size_t& offset);
    };
}

#endif //LIBLR1_CONV_JAMEXTRACTOR_HPP
