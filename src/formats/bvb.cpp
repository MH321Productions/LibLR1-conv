#include <iostream>

#include <LR1/formats/bvb.hpp>

using namespace std;
using namespace filesystem;

namespace LR1 {
    std::optional<Mesh> BvbDecoder::decode() {
        return nullopt;
    }

    bool BvbDecoder::save(const path& path, const Mesh& decoded) {
        return false;
    }
}
