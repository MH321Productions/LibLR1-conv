/**
 * The BVB Format encodes a 3D mesh containing vertices, polygons, and material references
 */

#ifndef LIBLR1_CONV_BVB_HPP
#define LIBLR1_CONV_BVB_HPP

#include <vector>

#include <glm/glm.hpp>

#include <LR1/decoder.hpp>

namespace LR1 {
    struct Polygon {

    };

    struct PolygonRange {

    };

    struct Mesh {
        std::vector<std::string> materials;
        std::vector<glm::vec3> vertices;
        std::vector<Polygon> polygons;
        std::vector<PolygonRange> polygonRanges;
    };

    class BvbDecoder : public CompressedBinaryDecoder<ResourceType::Mesh, Mesh> {
        public:
            BvbDecoder() = default;

            bool save(const std::filesystem::path &path, const Mesh &decoded) override;

        private:
            std::optional<Mesh> decode() override;
    };
}

#endif //LIBLR1_CONV_BVB_HPP
