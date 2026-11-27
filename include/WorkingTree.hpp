#pragma once

#include "RepositoryPaths.hpp"

#include <string>
#include <vector>

namespace minigit {

// Lists the real files in the project folder as sorted relative paths using '/',
// skipping .minigit. Needed in M4 (commit) and M5 (status).
class WorkingTree {
public:
    explicit WorkingTree(const RepositoryPaths& paths);

    std::vector<std::string> listFiles() const;

private:
    RepositoryPaths paths_;
};

} // namespace minigit
