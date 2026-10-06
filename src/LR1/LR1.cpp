#include <LR1/LR1.hpp>
#include <LR1/formats/audio.hpp>
#include <LR1/formats/bmp.hpp>
#include <LR1/formats/bvb.hpp>
#include <LR1/formats/mdb.hpp>
#include <LR1/formats/srf.hpp>

using namespace std;

namespace LR1 {
    unique_ptr<Converter> getConverter(const ResourceType type) {
        switch (type) {
            case ResourceType::Image:
                return make_unique<BmpDecoder>();
            case ResourceType::Audio:
                return make_unique<AudioDecoder>();
            case ResourceType::Text:
                return make_unique<SrfDecoder>();
            case ResourceType::Mesh:
                return make_unique<BvbDecoder>();
            case ResourceType::Material:
                return make_unique<MdbDecoder>();

            case ResourceType::Model:
            default:
                return nullptr;
        }
    }
}
