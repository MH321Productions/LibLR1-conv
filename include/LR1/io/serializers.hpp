#ifndef LIBLR1_CONV_VECTORS_HPP
#define LIBLR1_CONV_VECTORS_HPP

#include <string>

#include <glm/fwd.hpp>

namespace LR1 {
    class BinaryReader;

    namespace Serializers {
        struct vec3 {
            static glm::vec3 read(BinaryReader& reader);
        };

        struct string {
            static std::string read(BinaryReader& reader);
        };
    }
}

#endif //LIBLR1_CONV_VECTORS_HPP
