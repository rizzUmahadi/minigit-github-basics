#pragma once

#include "FileSystem.hpp"
#include "RepositoryPaths.hpp"

#include <filesystem>

namespace minigit {

class MiniGitRepository {
public:
    explicit MiniGitRepository(const std::filesystem::path& root);
    void init();
    bool isInitialized() const;

private:
    RepositoryPaths paths_;
    FileSystem fileSystem_;
};

} // namespace minigit