#include <iostream>
#include <fstream>

#include <LR1/formats/bvb.hpp>
#include <LR1/io/exceptions.hpp>

using namespace std;
using namespace filesystem;

namespace LR1 {
    std::optional<Mesh> BvbDecoder::decode() {
        Mesh mesh{};

        while (reader.position() < reader.size()) {
            switch (const uint8_t blockId = reader.readByte()) {
                case IdMaterials:
                    mesh.materials = reader.readStringArrayBlock();
                    break;

                case IdPolygons:
                    mesh.polygons = reader.readArrayBlock<Polygon>();
                    break;

                case IdVertices:
                    mesh.vertices = reader.readVector3fArrayBlock();
                    break;

                case IdPolygonRanges:
                    mesh.polygonRanges = reader.readArrayBlock<PolygonRange>();
                    break;

                default:
                    throw UnexpectedBlockException(blockId, reader.position() - 1);
            }
        }

        return mesh;
    }

    Polygon Polygon::read(BinaryReader& reader) {
        Polygon val{};
        val.v0 = reader.readIntegralWithHeader();
        val.v1 = reader.readIntegralWithHeader();
        val.v2 = reader.readIntegralWithHeader();
        val.material = reader.readIntegralWithHeader();

        return val;
    }

    PolygonRange PolygonRange::read(BinaryReader& reader) {
        PolygonRange val{};
        val.child0 = reader.readIntegralWithHeader();
        val.child1 = reader.readIntegralWithHeader();
        val.planeNormalXFixed = reader.readIntegralWithHeader();
        val.planeNormalYFixed = reader.readIntegralWithHeader();
        val.planeNormalZFixed = reader.readIntegralWithHeader();
        val.firstPolygonIndex = reader.readIntegralWithHeader();
        val.polygonCount = reader.readIntegralWithHeader();

        val.planeNormalX = static_cast<float>(val.planeNormalXFixed) / 1073741824.0f;
        val.planeNormalY = static_cast<float>(val.planeNormalYFixed) / 1073741824.0f;
        val.planeNormalZ = static_cast<float>(val.planeNormalZFixed) / 1073741824.0f;

        return val;
    }

    bool BvbDecoder::save(const path& path, const Mesh& decoded) {
        ofstream file(path);
        if (!file) return false;

        for (const glm::vec3& v: decoded.vertices) {
            file << "v " << v.x << " " << v.y << " " << v.z << "\n";
        }

        int currentMaterial = -1;
        for (const Polygon& p: decoded.polygons) {
            if (p.material != currentMaterial) {
                currentMaterial = p.material;
                file << "usemtl " << decoded.materials.at(p.material) << '\n';
            }

            file << "f " << p.v0 + 1 << " " << p.v1 + 1 << " " << p.v2 + 1 << "\n";
        }

        file.close();

        return true;
    }
}
