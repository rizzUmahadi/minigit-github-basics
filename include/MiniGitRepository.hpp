#pragma once

#include "FileSystem.hpp"
#include "RepositoryPaths.hpp"

#include <filesystem>
#include <string>

namespace minigit {

class MiniGitRepository {
public:
    explicit MiniGitRepository(const std::filesystem::path& root);

    // M2: minigit init
    void init();
    bool isInitialized() const;

    // Declared now so the design is visible; bodies come in later milestones.
    void add(const std::string& file);               // M3
    void commit(const std::string& message);         // M4
    void log() const;                                // M4
    void status() const;                             // M5

private:
    RepositoryPaths paths_;
    FileSystem fileSystem_;
};

} // namespace minigit