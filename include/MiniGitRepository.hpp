#pragma once

#include <filesystem>

#include "FileSystem.hpp"
#include "RepositoryPaths.hpp"

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