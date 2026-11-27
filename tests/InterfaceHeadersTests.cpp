// Compile check for the interface headers added in HDR-1.
// Including every header makes CI fail if any declaration does not compile.
// Only CommitInfo is used, because the other classes have no bodies until M3-M5.
#include "CommitInfo.hpp"
#include "CommitStore.hpp"
#include "FileState.hpp"
#include "HeadRef.hpp"
#include "IndexTable.hpp"
#include "MiniGitRepository.hpp"
#include "WorkingTree.hpp"

#include <iostream>

int main() {
    const minigit::CommitInfo first{"1", "first commit", std::nullopt};
    if (first.id != "1" || first.message != "first commit" || first.parent.has_value()) {
        std::cerr << "CommitInfo first-commit test failed\n";
        return 1;
    }

    const minigit::CommitInfo second{"2", "second commit", std::string("1")};
    if (!second.parent.has_value() || *second.parent != "1") {
        std::cerr << "CommitInfo parent test failed\n";
        return 1;
    }

    std::cout << "Interface headers compile test passed\n";
    return 0;
}
