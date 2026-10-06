#ifndef LIBLR1_CONV_TYPES_HPP
#define LIBLR1_CONV_TYPES_HPP

#include <string>
#include <filesystem>
#include <map>

namespace LR1 {
    /**
     * The resource types currently supported with a decoder
     */
    enum class ResourceType: uint8_t {
        Image,
        Audio,
        Text,
        Model,
        Mesh,
        Material,


        /**
         * The catch type for everything without a decoder (yet).
         * It also indicates the number of supported resource types,
         * therefore it has to be placed at the end
         */
        Unsupported
    };

    ResourceType getResourceType(const std::string& filename);
    ResourceType getResourceType(const std::filesystem::path& path);

    namespace Types {
        extern const std::map<std::string, ResourceType> extensionMap;
    }
}

#endif //LIBLR1_CONV_TYPES_HPP
