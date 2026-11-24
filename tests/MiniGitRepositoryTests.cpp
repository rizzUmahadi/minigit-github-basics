#include "FileSystem.hpp"
#include "MiniGitRepository.hpp"

#include <chrono>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace fs = std::filesystem;

static fs::path makeTempRoot(const std::string& name) {
    const auto stamp = std::chrono::steady_clock::now().time_since_epoch().count();
    const fs::path root = fs::temp_directory_path() /
        ("minigit-repo-" + name + "-" + std::to_string(stamp));
    fs::create_directories(root);
    return root;
}

static int fail(const fs::path& root, const std::string& message) {
    std::cerr << message << '\n';
    fs::remove_all(root);
    return 1;
}

int main() {
    minigit::FileSystem fileSystem;

    // Test 1 and 2: fresh folder, then init creates the full layout
    {
        const fs::path root = makeTempRoot("fresh");
        minigit::MiniGitRepository repo(root);

        if (repo.isInitialized()) {
            return fail(root, "isInitialized should be false before init");
        }

        repo.init();

        if (!repo.isInitialized()) {
            return fail(root, "isInitialized should be true after init");
        }
        if (!fs::is_directory(root / ".minigit" / "stage") ||
            !fs::is_directory(root / ".minigit" / "commits")) {
            return fail(root, "init did not create stage/ and commits/");
        }
        if (fs::file_size(root / ".minigit" / "HEAD") != 0 ||
            fs::file_size(root / ".minigit" / "index.txt") != 0) {
            return fail(root, "HEAD and index.txt should be empty after init");
        }
        fs::remove_all(root);
        std::cout << "init creates layout test passed\n";
    }

    // Test 3: second init throws and changes nothing
    {
        const fs::path root = makeTempRoot("twice");
        minigit::MiniGitRepository repo(root);
        repo.init();

        const std::vector<char> head = {'1'};
        fileSystem.writeBytes(root / ".minigit" / "HEAD", head);

        bool threw = false;
        try {
            repo.init();
        } catch (const std::runtime_error&) {
            threw = true;
        }
        if (!threw) {
            return fail(root, "second init should throw");
        }
        if (fileSystem.readBytes(root / ".minigit" / "HEAD") != head) {
            return fail(root, "second init changed HEAD");
        }
        fs::remove_all(root);
        std::cout << "init twice test passed\n";
    }

    // Test 4: only .minigit/ exists (damaged repo)
    {
        const fs::path root = makeTempRoot("partial");
        fs::create_directories(root / ".minigit");
        minigit::MiniGitRepository repo(root);

        if (repo.isInitialized()) {
            return fail(root, "isInitialized should be false without HEAD and index.txt");
        }
        bool threw = false;
        try {
            repo.init();
        } catch (const std::runtime_error&) {
            threw = true;
        }
        if (!threw) {
            return fail(root, "init should throw when .minigit already exists");
        }
        fs::remove_all(root);
        std::cout << "partial repository test passed\n";
    }

    // Test 5: existing project files are not touched by init
    {
        const fs::path root = makeTempRoot("project");
        const