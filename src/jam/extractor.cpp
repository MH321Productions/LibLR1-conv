/*
 * This is a C++ port from JrMasterBuilder's JAM-Extractor, which can be found here: https://github.com/JrMasterModelBuilder/JAM-Extractor
 */

#include <iostream>
#include <fstream>

#include <LR1/jam/extractor.hpp>

using namespace std;
using namespace std::filesystem;

namespace LR1 {

    optional<JamDirectory> JamExtractor::loadJam(const path& p) {
        buf = BinaryReader(p);

        if (buf.readString(4) != "LJAM") {
            cerr << "The file " << p << " is not a JAM file" << endl;
            return nullopt;
        }

        cout << "Extracting JAM File" << endl;

        JamDirectory root(p.filename().string(), 4);
        recurseChildren(root, 4);

        return root;
    }

    void JamExtractor::recurseChildren(JamDirectory& dir, const size_t& offset) {
        buf.seek(offset);
        uint32_t numChildFiles = buf.readUInt();
        if (!numChildFiles) { //No child files, only folders
            for (JamDirectory& subdir : listDirectories(buf.readUInt(), offset + 8)) {
                recurseChildren(subdir, subdir.offset);
                dir.addChild(subdir);
            }
        } else {
            for (JamFile& subfile: listFiles(numChildFiles, offset + 4)) {
                dir.addChild(subfile);
            }

            const size_t folderCountPos = numChildFiles * 20 + offset + 4;
            buf.seek(folderCountPos);
            uint32_t folderCount = buf.readUInt();
            if (folderCount) {
                for (JamDirectory& subdir : listDirectories(folderCount, folderCountPos + 4)) {
                    recurseChildren(subdir, subdir.offset);
                    dir.addChild(subdir);
                }
            }
        }
    }

    vector<JamFile> JamExtractor::listFiles(const uint32_t& number, const size_t& offset) {
        vector<JamFile> res;
        for (uint32_t i = 0; i < number; i++) {
            const size_t currentOffset = offset + i * 20;
            buf.seek(currentOffset);
            const string filename = buf.readString(12);
            const uint32_t contentOffset = buf.readUInt();
            const uint32_t contentSize = buf.readUInt();
            buf.seek(contentOffset);
            const vector<uint8_t> content = buf.readBytes(contentSize);

            JamFile f(filename, content, getResourceType(filename));
            res.push_back(f);
        }

        return res;
    }

    vector<JamDirectory> JamExtractor::listDirectories(const uint32_t& number, const size_t& offset) {
        vector<JamDirectory> res;
        res.reserve(number);
        for (uint32_t i = 0; i < number; i++) {
            const size_t currentOffset = offset + i * 16;
            buf.seek(currentOffset);
            const string dirname = buf.readString(12);
            const size_t dirOffset = buf.readUInt();
            JamDirectory dir(dirname, dirOffset);
            res.push_back(dir);
        }

        return res;
    }

}