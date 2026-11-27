#pragma once

#include "FileSystem.hpp"

#include <filesystem>
#include <optional>
#include <string>

namespace minigit {

// Reads and updates .minigit/HEAD, which holds the latest commit ID (empty before the first commit).
// Needed in M4 (commit, log); also read by status (M5).
class HeadRef {
public:
    explicit HeadRef(const std::filesystem::path& headFile);

    std::optional<std::string> read() const;         // no value when HEAD is empty
    void update(const std::string& id) const;

private:
    std::filesystem::path headFile_;
    FileSystem fileSystem_;
};

} // namespace minigit
