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
        int v0, v1, v2;
        int material;

        static Polygon read(BinaryReader& reader);
    };

    struct PolygonRange {
        int child0, child1;
        int planeNormalXFixed, planeNormalYFixed, planeNormalZFixed;
        float planeNormalX, planeNormalY, planeNormalZ;
        int firstPolygonIndex, polygonCount;

        static PolygonRange read(BinaryReader& reader);
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

        protected:
            std::optional<Mesh> decode() override;

        private:
            static constexpr uint8_t IdMaterials = 0x27;
            static constexpr uint8_t IdPolygons = 0x2D;
            static constexpr uint8_t IdVertices = 0x34;
            static constexpr uint8_t IdPolygonRanges = 0x8E;
    };
}

#endif //LIBLR1_CONV_BVB_HPP
