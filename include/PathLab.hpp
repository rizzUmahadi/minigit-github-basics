#pragma once
#include <filesystem>
#include <string>

namespace minigit {

// Week 1 Issues will add small helpers for building paths below a repository root.
// Path construction describes locations; it does not create files or directories.
    std::filesystem::path metadataDirectory(const std::filesystem::path& root);

    std::filesystem::path headFilePath(const std::filesystem::path& root);


    // Creates the parent folder of filePath if there isn't one, then writes text into the file.
    // Returns true on success, false if creating the folder or writing fails.
    bool writeText(const std::filesystem::path& filePath, const std::string &text);

} // namespace minigit
