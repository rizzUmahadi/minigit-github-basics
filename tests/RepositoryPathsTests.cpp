#include "RepositoryPaths.hpp"
#include <iostream>
#include <cassert>
#include <stdexcept>
#include <string>
#include <fstream>
#include <system_error>
// Returns true if safeRelativeFile throws std::runtime_error for this argument.
bool rejects(const minigit::RepositoryPaths& paths, const std::string& argument) {
    try {
        paths.safeRelativeFile(argument);
    } catch (const std::runtime_error&) {
        return true;
    }
    return false;
}

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
    // RP-4: commit directory
    if (nestedPaths.commitDirectory("3") != nestedRoot / ".minigit" / "commits" / "3") {
        std::cerr << "commitDirectory test failed\n";
        return 1;
    }

    // RP-4: committed file in a sub-folder
    if (nestedPaths.committedFile("3", "docs/notes.txt") != nestedRoot / ".minigit" / "commits" / "3" / "files" / "docs" / "notes.txt") {
        std::cerr << "committedFile sub-folder test failed\n";
        return 1;
    }

    // RP-4: committedFile reuses commitDirectory
    if (nestedPaths.committedFile("3", "a.txt").parent_path() != nestedPaths.commitDirectory("3") / "files") {
        std::cerr << "committedFile parent path test failed\n";
        return 1;
    }
    std::cout << "RepositoryPaths commit path tests passed\n";

      // RP-5: accepted paths return a clean relative path
    if (paths.safeRelativeFile("notes.txt") != "notes.txt" ||
        paths.safeRelativeFile("docs/notes.txt") != "docs/notes.txt" ||
        paths.safeRelativeFile("docs/./notes.txt") != "docs/notes.txt" ||
        paths.safeRelativeFile("docs/../notes.txt") != "notes.txt" ||
        paths.safeRelativeFile("my..notes.txt") != "my..notes.txt") {
        std::cerr << "safeRelativeFile accepted paths test failed\n";
        return 1;
    }
    std::cout << "safeRelativeFile accepted paths test passed\n";

    // RP-5: unsafe paths must throw std::runtime_error
    if (!rejects(paths, "") ||
        !rejects(paths, ".") ||
        !rejects(paths, "/notes.txt") ||
        !rejects(paths, "../notes.txt") ||
        !rejects(paths, "docs/../../notes.txt") ||
        !rejects(paths, ".minigit/HEAD") ||
        !rejects(paths, ".git/config") ||
        !rejects(paths, "docs/.minigit/x")) {
        std::cerr << "safeRelativeFile failed to reject an unsafe path\n";
        return 1;
    }
    std::cout << "safeRelativeFile rejected paths test passed\n";

        // PR-6: symbolic links are rejected
    const std::filesystem::path linkRoot = std::filesystem::temp_directory_path() / "minigit_symlink_test";
    std::error_code linkError;
    std::filesystem::remove_all(linkRoot, linkError);
    std::filesystem::create_directories(linkRoot / "docs");
    std::ofstream(linkRoot / "real.txt") << "real\n";
    std::ofstream(linkRoot / "docs" / "notes.txt") << "notes\n";
    const minigit::RepositoryPaths linkPaths(linkRoot);

    std::error_code fileLinkError;
    std::error_code folderLinkError;
    std::filesystem::create_symlink("real.txt", linkRoot / "file_link.txt", fileLinkError);
    std::filesystem::create_directory_symlink("docs", linkRoot / "folder_link", folderLinkError);

    bool linkTestFailed = false;
    if (fileLinkError || folderLinkError) {
        std::cout << "symbolic link test skipped\n";
    } else {
        if (rejects(linkPaths, "real.txt")) {
            std::cerr << "symbolic link test failed: real file was rejected\n";
            linkTestFailed = true;
        }
        if (!rejects(linkPaths, "file_link.txt")) {
            std::cerr << "symbolic link test failed: file link was accepted\n";
            linkTestFailed = true;
        }
        if (!rejects(linkPaths, "folder_link/notes.txt")) {
            std::cerr << "symbolic link test failed: file through folder link was accepted\n";
            linkTestFailed = true;
        }
        if (!linkTestFailed) {
            std::cout << "symbolic link test passed\n";
        }
    }
    std::filesystem::remove_all(linkRoot, linkError);
    if (linkTestFailed) {
        return 1;
    }
    return 0;
}
