#include <map>

#include <LR1/types.hpp>

using namespace std;
using namespace std::filesystem;

namespace LR1 {
    const map<string, ResourceType> Types::extensionMap = {
        {".BMP", ResourceType::Image},
        {".TUN", ResourceType::Audio},
        {".tun", ResourceType::Audio},
        {".PCM", ResourceType::Audio},
        {".SRF", ResourceType::Text},
        {".GDB", ResourceType::Model},
        {".BVB", ResourceType::Mesh},
        {".MDB", ResourceType::Material}
    };

    static ResourceType mapExtension(const string& extension) {
        if (!Types::extensionMap.contains(extension)) return ResourceType::Unsupported;
        return Types::extensionMap.at(extension);
    }

    ResourceType getResourceType(const std::string& filename) {
        const size_t dotPos = filename.find_last_of('.');
        if (dotPos == string::npos) return ResourceType::Unsupported;
        return mapExtension(filename.substr(dotPos));
    }

    ResourceType getResourceType(const path& path) {
        return mapExtension(path.extension().string());
    }
}
