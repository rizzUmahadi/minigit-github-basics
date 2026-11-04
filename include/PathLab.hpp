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

    // Reads the whole file into text.
    // Returns true on success, false if the file is missing or cannot be read.
    // text is only changed when the read succeeds.
    bool readText(const std::filesystem::path& filePath, std::string& text);

} // namespace minigit
