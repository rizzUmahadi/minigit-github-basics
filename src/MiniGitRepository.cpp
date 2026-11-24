#include "MiniGitRepository.hpp"

#include <stdexcept>
#include <system_error>

namespace minigit {

MiniGitRepository::MiniGitRepository(const std::filesystem::path& root)
    : paths_(root) {}

bool MiniGitRepository::isInitialized() const {
    std::error_code error;
    const bool hasMetadataDir = std::filesystem::is_directory(paths_.metadataRoot(), error);
    return hasMetadataDir
        && fileSystem_.fileExists(paths_.headFile())
        && fileSystem_.fileExists(paths_.indexFile());
}

void MiniGitRepository::init() {
    if (isInitialized()) {
        throw std::runtime_error("repository already initialized");
    }

    std::error_code error;
    if (std::filesystem::exists(paths_.metadataRoot(), error)) {
        throw std::runtime_error("damaged .minigit found; refusing to overwrite it");
    }

    fileSystem_.createDirectories(paths_.metadataRoot());
    fileSystem_.createDirectories(paths_.stageDir());
    fileSystem_.createDirectories(paths_.commitsDir());
    fileSystem_.createFile(paths_.headFile());
    fileSystem_.createFile(paths_.indexFile());
}

} // namespace minigit