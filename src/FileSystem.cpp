#include "FileSystem.hpp"
#include <system_error>
#include <algorithm>
#include <fstream>
#include <ios>
#include <limits>

namespace minigit {

void FileSystem::writeBytes(const std::filesystem::path& path, const std::vector<char>& data) const {
    std::ofstream file(path, std::ios::binary | std::ios::trunc);
    if (!file.is_open()) {
        throw std::ios_base::failure("Unable to open file for binary writing");
    }

    const auto maxChunk = static_cast<std::size_t>(std::numeric_limits<std::streamsize>::max());
    std::size_t offset = 0;
    while (offset < data.size()) {
        const auto chunk = std::min(maxChunk, data.size() - offset);
        file.write(data.data() + offset, static_cast<std::streamsize>(chunk));
        if (!file) {
            throw std::ios_base::failure("Unable to write all binary data");
        }
        offset += chunk;
    }

    file.close();
    if (!file) {
        throw std::ios_base::failure("Unable to close file after binary writing");
    }
}
std::vector<char> FileSystem::readBytes(const std::filesystem::path& path) const {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        throw std::ios_base::failure("Unable to open file for binary reading");
    }

    std::vector<char> data;
    char byte;
    while (file.get(byte)) {
        data.push_back(byte);
    }
    return data;
}

bool FileSystem::fileExists(const std::filesystem::path& path) const {
    std::error_code error;
    return std::filesystem::is_regular_file(path, error);
}

void FileSystem::copy(const std::filesystem::path& source, const std::filesystem::path& target) const {
    if (!fileExists(source)) {
        throw std::ios_base::failure("Source file does not exist: " + source.string());
    }
    writeBytes(target, readBytes(source));
}

void FileSystem::createDirectories(const std::filesystem::path& path) const {
    std::error_code error;
    std::filesystem::create_directories(path, error);
    if (error || !std::filesystem::is_directory(path)) {
        throw std::ios_base::failure("Unable to create directory");
    }
}

void FileSystem::createFile(const std::filesystem::path& path) const {
    std::ofstream file(path, std::ios::binary | std::ios::app);
    if (!file.is_open()) {
        throw std::ios_base::failure("Unable to create file");
    }
}

} // namespace minigit