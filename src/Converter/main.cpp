#include <vector>

#include <tinyfiledialogs/tinyfiledialogs.h>

#include <LR1/types.hpp>

#include "LR1/LR1.hpp"

using namespace std;

int main() {
    vector<const char*> types;
    for (const auto& p : LR1::Types::extensionMap) {
        types.push_back(("*" + p.first).c_str());
    }

    string file = tinyfd_openFileDialog(
        "title",
        nullptr,
        static_cast<int>(types.size()),
        types.data(),
        "Supported file types",
        0
    );

    if (file.empty()) return 0;

    LR1::ResourceType restype = LR1::getResourceType(file);
    if (restype == LR1::ResourceType::Unsupported) {
        tinyfd_messageBox("Not supported", "The selected file is not supported.", "ok", "error", 1);
        return 1;
    }

    LR1::DecoderFactory::getDecoder<restype, LR1::Bmp>();
}