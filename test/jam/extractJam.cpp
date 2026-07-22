#include <iostream>
#include <filesystem>

#include <LR1/jam/extractor.hpp>

using namespace std;
using namespace filesystem;

int ret = 0;

void weakAssert(const bool& cond, const string& failureMsg = "");
void assert(const bool& cond, const string& failureMsg = "");
int findFile(const LR1::JamDirectory& dir, const string& name);
int findDir(const LR1::JamDirectory& dir, const string& name);

int main(int argc, char* argv[]) {
    cout << "Testing JAM Extractor" << endl;

    if (argc != 2) {
        cout << "Usage: jamExtractor <path to file>" << endl;
        return 1;
    }

    const path jamPath(argv[1]);
    LR1::JamExtractor extractor;
    optional<LR1::JamDirectory> jamDir = extractor.loadJam(jamPath);

    assert(jamDir.has_value(), "The file couldn't be parsed");

    weakAssert(jamDir->childFiles.empty(), "The root directory shouldn't have child files");
    weakAssert(jamDir->childDirectories.size() == 2, "The root directory should have two child directories");

    const int gameIndex = findDir(jamDir.value(), "GAME");
    weakAssert(gameIndex != -1, "The root directory should contain the 'GAME' directory");

    const int menuIndex = findDir(jamDir.value(), "MENU");
    weakAssert(menuIndex != -1, "The root directory should contain the 'MENU' directory");

    if (gameIndex != -1) {
        LR1::JamDirectory game = jamDir->childDirectories.at(gameIndex);
        weakAssert(game.childFiles.size() == 1, "The 'GAME' directory should have 1 child file");
        weakAssert(game.childDirectories.empty(), "The 'GAME' directory should not have child directories");

        const int levelIndex = findFile(game, "LEVELS.TXT");
        weakAssert(levelIndex != -1, "The 'GAME' directory should contain the 'LEVELS.TXT' file");
        if (levelIndex != -1) {
            LR1::JamFile level = game.childFiles.at(levelIndex);
            string levelData(reinterpret_cast<const char*>(level.data.data()), level.data.size());
            weakAssert(levelData == "Mars, Desert, Alien\n", "The level data should be 'Mars, Desert, Alien\\n'");
        }
    }

    if (menuIndex != -1) {
        LR1::JamDirectory menu = jamDir->childDirectories.at(menuIndex);
        weakAssert(menu.childFiles.size() == 1, "The 'MENU' directory should have 1 child file");
        weakAssert(menu.childDirectories.empty(), "The 'MENU' directory should not have child directories");

        const int entryIndex = findFile(menu, "ENTRIES.TXT");
        weakAssert(entryIndex != -1, "The 'MENU' directory should contain the 'ENTRIES.TXT' file");
        if (entryIndex != -1) {
            LR1::JamFile entries = menu.childFiles.at(entryIndex);
            string levelData(reinterpret_cast<const char*>(entries.data.data()), entries.data.size());
            weakAssert(levelData == "Build, Settings, Quit\n", "The entry data should be 'Build, Settings, Quit\\n'");
        }
    }

    return ret;
}

int findFile(const LR1::JamDirectory& dir, const string& name) {
    for (int i = 0; i < dir.childFiles.size(); i++) {
        if (dir.childFiles.at(i).filename == name) return i;
    }
    return -1;
}

int findDir(const LR1::JamDirectory& dir, const string& name) {
    for (int i = 0; i < dir.childDirectories.size(); i++) {
        if (dir.childDirectories.at(i).name == name) return i;
    }
    return -1;
}

void weakAssert(const bool& cond, const string& failureMsg) {
    if (!cond) {
        cerr << failureMsg << endl;
        ret++;
    }
}

void assert(const bool& cond, const string& failureMsg) {
    if (!cond) {
        cerr << failureMsg << endl;
        exit(++ret);
    }
}