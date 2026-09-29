#include <glm/glm.hpp>

#include <LR1/io/serializers.hpp>
#include <LR1/io/reader.hpp>


namespace LR1::Serializers {
    glm::vec3 vec3::read(BinaryReader &reader) {
        return {reader.readFloatWithHeader(), reader.readFloatWithHeader(), reader.readFloatWithHeader()};
    }

    std::string string::read(BinaryReader& reader) {return reader.readStringWithHeader();}
}

