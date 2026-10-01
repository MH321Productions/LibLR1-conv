/**
 * The MDB format encodes a material database
 */

#ifndef LIBLR1_CONV_MDB_HPP
#define LIBLR1_CONV_MDB_HPP

#include <map>
#include <string>

#include <LR1/decoder.hpp>
#include <LR1/utils/color.hpp>

namespace LR1 {

    enum class MaterialBlendFactor: uint8_t {
        Zero = 0x39,
        One = 0x3A,
        SourceColor = 0x3B,
        DestinationColor = 0x3C,
        InverseSourceColor = 0x3D,
        InverseDestinationColor = 0x3E,
        SourceAlpha = 0x3F,
        DestinationAlpha = 0x40,
        InverseSourceAlpha = 0x41,
        InverseDestinationAlpha = 0x42,
        SourceAlphaSaturate = 0x43
    };

    struct MaterialBlend {
        MaterialBlendFactor srcFactor;
        MaterialBlendFactor dstFactor;

        static MaterialBlend read(BinaryReader& reader);
        static void validateToken(const uint8_t& token, const BinaryReader& reader);
    };

    enum class MaterialAlphaComparison: uint8_t {
        Always = 0x30,
        Equal = 0x31,
        Greater = 0x32,
        GreaterOrEqual = 0x33,
        Less = 0x34,
        LessOrEqual = 0x35,
        Never = 0x36,
        NotEqual = 0x37
    };

    struct MaterialAlphaTest {
        MaterialAlphaComparison comparison;
        std::optional<int32_t> referenceValue;

        static MaterialAlphaTest read(BinaryReader& reader);
        static MaterialAlphaComparison readComparison(BinaryReader& reader);
    };

    enum MaterialProperties: uint8_t {
        AmbientColor = 0x28,
        DiffuseColor = 0x29,
        FlatShading = 0x2A,
        GouraudShading = 0x2B,
        TextureName = 0x2C,
        Modulate = 0x2D,
        Decal = 0x2E,
        // Parser-supported but unobserved in installed MDB materials; keep structural.
        AlphaTest = 0x2F,
        Blend = 0x38,
        /*PROPERTY_3A = 0x3A,
        PROPERTY_3F = 0x3F,
        PROPERTY_41 = 0x41,*/
        LinearFilter = 0x44,
        PointFilter = 0x45,
        Alpha = 0x46,
        RotateVertices = 0x47,
        Wrap = 0x48,
        Clamp = 0x49,
        PROPERTY_4A = 0x4A,
        PROPERTY_4B = 0x4B,
        PROPERTY_4C = 0x4C,
        Transparency2 = 0x4D,
        Transparency3 = 0x4E,
        Transparency4 = 0x4F,
        Transparency5 = 0x50
    };

    struct Material {
        Color ambientColor, diffuseColor;
        bool flatShading, gouraudShading;
        std::string textureName;
        bool modulate, decal;
        MaterialAlphaTest alphaTest;
        MaterialBlend blend;
        bool linearFilter, pointFilter;
        bool rotateVertices;
        bool wrap, clamp;
        bool bool4A, bool4B, bool4C;
        std::optional<int32_t> transparency, transparency2, transparency3, transparency4, transparency5;

        static Material read(BinaryReader& reader);
    };

    class MdbDecoder : public CompressedBinaryDecoder<ResourceType::Material, std::map<std::string, Material>> {
        public:
            bool save(const std::filesystem::path &path, const std::map<std::string, Material> &decoded) override;

        protected:
            std::optional<std::map<std::string, Material>> decode() override;
    };
}

#endif //LIBLR1_CONV_MDB_HPP
