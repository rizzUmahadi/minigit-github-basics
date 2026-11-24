#pragma once

#include <filesystem>
#include <vector>

namespace minigit {

class FileSystem {
public:
    void writeBytes(const std::filesystem::path& path, const std::vector<char>& data) const;
    std::vector<char> readBytes(const std::filesystem::path& path) const;
    bool fileExists(const std::filesystem::path& path) const;
    void copy(const std::filesystem::path& source, const std::filesystem::path& target) const;
    void createDirectories(const std::filesystem::path& path) const;
    void createFile(const std::filesystem::path& path) const;
};
} // namespace minigit