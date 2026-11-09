#include "RepositoryPaths.hpp"
#include <iostream>
#include <cassert>

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
    if (paths.headFile() != root / ".minigit" / "HEAD") {
        std::cerr << "headFile ordinary root test failed\n";
        return 1;
    }
    if (paths.headFile().parent_path() != paths.metadataRoot()) {
        std::cerr << "headFile parent ordinary root test failed\n";
        return 1;
    }
    if (paths.indexFile() != root / ".minigit" / "index.txt") {
        std::cerr << "indexFile ordinary root test failed\n";
        return 1;
    }
    if (paths.indexFile().filename() != "index.txt") {
        std::cerr << "indexFile filename ordinary root test failed\n";
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
    if (nestedPaths.headFile() != nestedRoot / ".minigit" / "HEAD") {
        std::cerr << "headFile nested root test failed\n";
        return 1;
    }
    if (nestedPaths.headFile().parent_path() != nestedPaths.metadataRoot()) {
        std::cerr << "headFile parent nested root test failed\n";
        return 1;
    }
    if (nestedPaths.indexFile() != nestedRoot / ".minigit" / "index.txt") {
        std::cerr << "indexFile nested root test failed\n";
        return 1;
    }
    if (nestedPaths.indexFile().filename() != "index.txt") {
        std::cerr << "indexFile filename nested root test failed\n";
        return 1;
    }
    std::cout << "RepositoryPaths nested root test passed\n";
    
       // Test top-of-project file staging path
    if (nestedPaths.stageFile("notes.txt") != nestedRoot / ".minigit" / "stage" / "notes.txt") {
        std::cerr << "stageFile top-of-project path test failed\n";
        return 1;
    }

    // Test file in sub-folder staging path
    if (nestedPaths.stageFile("docs/notes.txt") != nestedRoot / ".minigit" / "stage" / "docs" / "notes.txt") {
        std::cerr << "stageFile sub-folder path test failed\n";
        return 1;
    }

    // Test filename extraction from staged path
    if (nestedPaths.stageFile("docs/notes.txt").filename() != "notes.txt") {
        std::cerr << "stageFile filename extraction test failed\n";
        return 1;
    }

    return 0;
}