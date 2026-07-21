#include <fstream>

#include <nlohmann/json.hpp>

#include <LR1/formats/srf.hpp>

using namespace std;
using namespace std::filesystem;
using json = nlohmann::json;

optional<vector<string> > LR1::SrfDecoder::decode() {
    const uint16_t numStrings = reader.readUShort();
    const uint16_t numChars = reader.readUShort();

    vector<uint16_t> offsets(numStrings);
    for (size_t i = 0; i < numStrings; i++) {
        offsets.at(i) = reader.readUShort();
    }

    vector<string> strings(numStrings);
    for (size_t i = 0; i < numStrings; i++) {
        strings.at(i) = reader.readWideString(2 * (offsets.at(i) + numStrings + 2));
    }

    return strings;
}

bool LR1::SrfDecoder::save(const path& path, const std::vector<std::string>& decoded) {
    json j(decoded);
    ofstream file(path);
    if (!file) return false;

    file << setw(4) << j << endl;
    file.close();

    return true;
}
