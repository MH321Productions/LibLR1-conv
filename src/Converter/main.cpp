#include <vector>
#include <format>
#include <filesystem>

#include <tinyfiledialogs/tinyfiledialogs.h>

#include <LR1/LR1.hpp>

using namespace std;
using namespace std::filesystem;

int main() {
    vector<string> types;
    vector<const char*> ctypes;
    for (const auto& p : LR1::Types::inputExtensionMap) {
        types.push_back(format("*{}", p.first));
    }

    for (const string& t: types) ctypes.push_back(t.c_str());

    const char* inFile = tinyfd_openFileDialog(
        "Select the file to convert from",
        nullptr,
        static_cast<int>(ctypes.size()),
        ctypes.data(),
        "Supported file types",
        0
    );

    if (!inFile) return 0;

    const LR1::ResourceType type = LR1::getResourceType(string(inFile));
    const unique_ptr<LR1::Converter> converter = LR1::getConverter(type);
    if (!converter) {
        tinyfd_messageBox("Not supported", "The selected file is not supported.", "ok", "error", 1);
        return 1;
    }

    const string saveExtension = "*" + LR1::Types::outputExtensionMap.at(type);
    const char* cSaveExtension = saveExtension.c_str();
    const char* outFile = tinyfd_saveFileDialog(
        "Select the file to save to",
        nullptr,
        1,
        &cSaveExtension,
        nullptr
    );

    if (!outFile) return 0;

    if (converter->convert(path(inFile), path(outFile))) {
        tinyfd_messageBox("Conversion successful", "The file has been converted successfully", "ok", "info", 1);
        return 0;
    }

    tinyfd_messageBox("Conversion failed", "The file could not be converted", "ok", "error", 1);
    return 1;
}
