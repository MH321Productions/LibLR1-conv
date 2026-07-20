/*
 * This is a C++ port from JrMasterBuilder's JAM-Extractor, which can be found here: https://github.com/JrMasterModelBuilder/JAM-Extractor
 */

#include <iostream>
#include <fstream>

#include <LR1/jam/JamExtractor.hpp>

using namespace std;
using namespace std::filesystem;

namespace LR1 {

    optional<JamDirectory> JamExtractor::loadJam(const path& p) {
        buf = BinaryReader(p);

        if (buf.readAsciiString(4, 0) != "LJAM") {
            cerr << "The file " << p << " is not a JAM file" << endl;
            return nullopt;
        }

        cout << "Extracting JAM File" << endl;

        JamDirectory root(p.filename().string(), 4);
        recurseChildren(root, 4);

        return root;
    }

    void JamExtractor::recurseChildren(JamDirectory& dir, const size_t& offset) {
        uint32_t numChildFiles = buf.readUInt(offset);
        if (!numChildFiles) { //No child files, only folders
            for (JamDirectory& subdir : listDirectories(buf.readUInt(offset + 4), offset + 8)) {
                recurseChildren(subdir, subdir.offset);
                dir.addChild(subdir);
            }
        } else {
            for (JamFile& subfile: listFiles(numChildFiles, offset + 4)) {
                dir.addChild(subfile);
            }

            const size_t folderCountPos = numChildFiles * 20 + offset + 4;
            uint32_t folderCount = buf.readUInt(folderCountPos);
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
            const string filename = buf.readAsciiString(12, currentOffset);
            const uint32_t contentOffset = buf.readUInt(currentOffset + 12);
            const uint32_t contentSize = buf.readUInt(currentOffset + 16);
            const vector<uint8_t> content = buf.readBuffer<uint8_t>(contentSize, contentOffset);

            JamFile f(filename, content);
            res.push_back(f);
        }

        return res;
    }

    vector<JamDirectory> JamExtractor::listDirectories(const uint32_t& number, const size_t& offset) {
        vector<JamDirectory> res;
        res.reserve(number);
        for (uint32_t i = 0; i < number; i++) {
            const size_t currentOffset = offset + i * 16;
            const string dirname = buf.readAsciiString(12, currentOffset);
            const size_t dirOffset = buf.readUInt(currentOffset + 12);
            JamDirectory dir(dirname, dirOffset);
            res.push_back(dir);
        }

        return res;
    }

}