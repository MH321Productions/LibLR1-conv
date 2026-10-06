#include <LR1/jam/extractor.hpp>

using namespace std;
using namespace std::filesystem;

namespace LR1 {
    /* JamFile */
    std::string JamFile::extension() const {
        const size_t dotPos = filename.find_last_of('.');
        if (dotPos == string::npos) return "";
        return filename.substr(dotPos);
    }
    std::string JamFile::stem() const {return filename.substr(0, filename.find_last_of('.'));}

    /* JamDirectory */
    void JamDirectory::addChild(JamFile& file) {
        childFiles.push_back(file);
        file.parent = this;
    }

    void JamDirectory::addChild(JamDirectory& dir) {
        childDirectories.push_back(dir);
        dir.parent = this;
    }
}