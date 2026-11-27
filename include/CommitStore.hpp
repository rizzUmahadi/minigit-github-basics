#pragma once

#include "CommitInfo.hpp"
#include "FileSystem.hpp"
#include "RepositoryPaths.hpp"

#include <optional>
#include <string>
#include <vector>

namespace minigit {

// Saves and reads commits under .minigit/commits/<id>/. Needed in M4 (commit, log).
class CommitStore {
public:
    explicit CommitStore(const RepositoryPaths& paths);

    std::string nextId() const;                      // "1" for the first commit, then counts up
    void create(const std::string& id,
                const std::vector<std::string>& files,
                const std::string& message,
                const std::optional<std::string>& parent) const;
    CommitInfo info(const std::string& id) const;
    std::vector<CommitInfo> history(const std::string& startId) const;  // newest first

private:
    RepositoryPaths paths_;
    FileSystem fileSystem_;
};

} // namespace minigit
