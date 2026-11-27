#pragma once

#include <optional>
#include <string>

namespace minigit {

// One saved commit: its ID, message and the ID of the commit before it.
// The first commit has no parent. Needed in M4 (commit, log).
struct CommitInfo {
    std::string id;
    std::string message;
    std::optional<std::string> parent;
};

} // namespace minigit
