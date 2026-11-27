#pragma once

#include "FileSystem.hpp"

#include <filesystem>
#include <string>
#include <vector>

namespace minigit {

// The list of tracked files stored in .minigit/index.txt, one relative path per line.
// Needed in M3 (add); also read by commit (M4) and status (M5).
class IndexTable {
public:
    explicit IndexTable(const std::filesystem::path& indexFile);

    void load();                                     // a missing index.txt loads as an empty list
    void save() const;                               // writes the sorted list back to index.txt
    std::vector<std::string> files() const;
    bool contains(const std::string& path) const;
    void add(const std::string& path);               // adds once, keeps the list sorted

private:
    std::filesystem::path indexFile_;
    std::vector<std::string> entries_;
    FileSystem fileSystem_;
};

} // namespace minigit
