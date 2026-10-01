#include <LR1/io/reader.hpp>
#include <LR1/formats/mdb.hpp>
#include <LR1/io/exceptions.hpp>

namespace LR1 {
    MaterialBlend MaterialBlend::read(BinaryReader& reader) {
        MaterialBlend blend{};

        uint8_t token = reader.readByte();
        validateToken(token, reader);
        blend.srcFactor = static_cast<MaterialBlendFactor>(token);

        token = reader.readByte();
        validateToken(token, reader);
        blend.dstFactor = static_cast<MaterialBlendFactor>(token);

        return blend;
    }

    void MaterialBlend::validateToken(const uint8_t& token, const BinaryReader& reader) {
        if (token < static_cast<uint8_t>(MaterialBlendFactor::Zero) || token > static_cast<uint8_t>(MaterialBlendFactor::SourceAlphaSaturate))
            throw UnexpectedPropertyException(token, reader.position() - 1);
    }

    MaterialAlphaTest MaterialAlphaTest::read(BinaryReader& reader) {
        MaterialAlphaTest res{};
        res.comparison = readComparison(reader);
        switch (res.comparison) {
            case MaterialAlphaComparison::Always:
            case MaterialAlphaComparison::Never:
                //Bare flags, no value
                break;

            case MaterialAlphaComparison::Equal:
            case MaterialAlphaComparison::Greater:
            case MaterialAlphaComparison::GreaterOrEqual:
            case MaterialAlphaComparison::Less:
            case MaterialAlphaComparison::LessOrEqual:
            case MaterialAlphaComparison::NotEqual:
                res.referenceValue = reader.readIntWithHeader();
                break;
        }

        return res;
    }

    MaterialAlphaComparison MaterialAlphaTest::readComparison(BinaryReader& reader) {
        uint8_t token = reader.readByte();
        if (token < static_cast<uint8_t>(MaterialAlphaComparison::Always) || token > static_cast<uint8_t>(MaterialAlphaComparison::NotEqual))
            throw UnexpectedPropertyException(token, reader.position() - 1);

        return static_cast<MaterialAlphaComparison>(token);
    }

    Material Material::read(BinaryReader& reader) {
        Material mat{};
        while (!reader.next(Token::RightCurly)) {
            switch (const uint8_t property = reader.readByte()) {
                case AmbientColor:
                    mat.ambientColor = reader.readSerializable<Color>();
                    break;

                case DiffuseColor:
                    mat.diffuseColor = reader.readSerializable<Color>();
                    break;

                case FlatShading:
                    mat.flatShading = true;
                    break;

                case GouraudShading:
                    mat.gouraudShading = true;
                    break;

                case TextureName:
                    mat.textureName = reader.readStringWithHeader();
                    break;

                case Modulate:
                    mat.modulate = true;
                    break;

                case Decal:
                    mat.decal = true;
                    break;

                case AlphaTest:
                    mat.alphaTest = reader.readSerializable<MaterialAlphaTest>();
                    break;

                case Blend:
                    mat.blend = reader.readSerializable<MaterialBlend>();
                    break;

                case LinearFilter:
                    mat.linearFilter = true;
                    break;

                case PointFilter:
                    mat.pointFilter = true;
                    break;

                case RotateVertices:
                    mat.rotateVertices = true;
                    break;

                case Wrap:
                    mat.wrap = true;
                    break;

                case Clamp:
                    mat.clamp = true;
                    break;

                case PROPERTY_4A:
                    mat.bool4A = true;
                    break;

                case PROPERTY_4B:
                    mat.bool4B = true;
                    break;

                case PROPERTY_4C:
                    mat.bool4C = true;
                    break;

                case Alpha:
                    mat.transparency = reader.readIntWithHeader();
                    break;

                case Transparency2:
                    mat.transparency2 = reader.readIntWithHeader();
                    break;

                case Transparency3:
                    mat.transparency3 = reader.readIntWithHeader();
                    break;

                case Transparency4:
                    mat.transparency4 = reader.readIntWithHeader();
                    break;

                case Transparency5:
                    mat.transparency5 = reader.readIntWithHeader();
                    break;

                default:
                    throw UnexpectedPropertyException(property, reader.position() - 1);
            }
        }

        return mat;
    }
}
