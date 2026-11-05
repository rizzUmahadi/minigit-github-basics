#include "RepositoryPaths.hpp"
#include <iostream>

int main() {
    // RP-1: ordinary root
    const std::filesystem::path root = std::filesystem::path("course") / "repo";
    const minigit::RepositoryPaths paths(root);
    if (paths.projectRoot() != root) {
        std::cerr << "projectRoot ordinary root test failed\n";
        return 1;
    }
    if (paths.metadataRoot() != root / ".minigit") {
        std::cerr << "metadataRoot ordinary root test failed\n";
        return 1;
    }
    std::cout << "RepositoryPaths ordinary root test passed\n";

    // RP-1: nested root
    const std::filesystem::path nestedRoot = std::filesystem::path("course") / "team" / "repo";
    const minigit::RepositoryPaths nestedPaths(nestedRoot);
    if (nestedPaths.projectRoot() != nestedRoot) {
        std::cerr << "projectRoot nested root test failed\n";
        return 1;
    }
    if (nestedPaths.metadataRoot() != nestedRoot / ".minigit") {
        std::cerr << "metadataRoot nested root test failed\n";
        return 1;
    }
    std::cout << "RepositoryPaths nested root test passed\n";

    return 0;
}