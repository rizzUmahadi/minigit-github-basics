#include "MiniGitRepository.hpp"

#include <stdexcept>
#include <system_error>

namespace minigit {

MiniGitRepository::MiniGitRepository(const std::filesystem::path& root)
    : paths_(root) {}

bool MiniGitRepository::isInitialized() const {
    std::error_code error;
    const bool hasFolder = std::filesystem::is_directory(paths_.metadataRoot(), error);
    return hasFolder
        && fileSystem_.fileExists(paths_.headFile())
        && fileSystem_.fileExists(paths_.indexFile());
}

void MiniGitRepository::init() {
    std::error_code error;
    if (std::filesystem::exists(paths_.metadataRoot(), error)) {
        throw std::runtime_error("MiniGit repository already exists in "
                                 + paths_.projectRoot().string());
    }

    fileSystem_.createDirectories(paths_.stageDir());
    fileSystem_.createDirectories(paths_.commitsDir());
    fileSystem_.createFile(paths_.headFile());
    fileSystem_.createFile(paths_.indexFile());
}

} // namespace minigit