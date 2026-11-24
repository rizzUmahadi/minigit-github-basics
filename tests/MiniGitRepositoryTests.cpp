#include "FileSystem.hpp"
#include "MiniGitRepository.hpp"

#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace fs = std::filesystem;

namespace {

fs::path freshTempDir(const char* name) {
    const fs::path dir = fs::temp_directory_path() / name;
    fs::remove_all(dir);
    fs::create_directories(dir);
    return dir;
}

int fail(const char* message, const fs::path& dir) {
    std::cerr << message << "\n";
    fs::remove_all(dir);
    return 1;
}

} // namespace

int main() {
    const minigit::FileSystem fileSystem;

    // Empty folder: not initialized
    {
        const fs::path root = freshTempDir("mgr_test_empty");
        const minigit::MiniGitRepository repo(root);
        if (repo.isInitialized()) return fail("empty folder test failed: expected false", root);
        fs::remove_all(root);
    }
    std::cout << "Empty folder test passed\n";

    // Only .minigit, no HEAD or index.txt: not initialized
    {
        const fs::path root = freshTempDir("mgr_test_partial");
        fs::create_directories(root / ".minigit");
        const minigit::MiniGitRepository repo(root);
        if (repo.isInitialized()) return fail("partial .minigit test failed: expected false", root);
        fs::remove_all(root);
    }
    std::cout << "Partial .minigit test passed\n";

    // Fresh init creates the full layout, with empty HEAD and index.txt
    {
        const fs::path root = freshTempDir("mgr_test_init");
        minigit::MiniGitRepository repo(root);
        repo.init();
        const fs::path meta = root / ".minigit";
        if (!fs::is_directory(meta)) return fail("init test failed: .minigit missing", root);
        if (!fs::is_directory(meta / "stage")) return fail("init test failed: stage/ missing", root);
        if (!fs::is_directory(meta / "commits")) return fail("init test failed: commits/ missing", root);
        if (!fs::is_regular_file(meta / "HEAD")) return fail("init test failed: HEAD missing", root);
        if (!fs::is_regular_file(meta / "index.txt")) return fail("init test failed: index.txt missing", root);
        if (fs::file_size(meta / "HEAD") != 0) return fail("init test failed: HEAD not empty", root);
        if (fs::file_size(meta / "index.txt") != 0) return fail("init test failed: index.txt not empty", root);
        if (!repo.isInitialized()) return fail("init test failed: isInitialized() false after init", root);
        fs::remove_all(root);
    }
    std::cout << "Fresh init test passed\n";

    // Init twice: throws and changes nothing
    {
        const fs::path root = freshTempDir("mgr_test_twice");
        minigit::MiniGitRepository repo(root);
        repo.init();
        const fs::path head = root / ".minigit" / "HEAD";
        const std::vector<char> marker = {'0', '0', '1'};
        fileSystem.writeBytes(head, marker);

        bool threw = false;
        try {
            repo.init();
        } catch (const std::runtime_error&) {
            threw = true;
        }
        if (!threw) return fail("init twice test failed: second init did not throw", root);
        if (fileSystem.readBytes(head) != marker) return fail("init twice test failed: HEAD was changed", root);
        fs::remove_all(root);
    }
    std::cout << "Init twice test passed\n";

    // Damaged .minigit (HEAD deleted): not initialized, init throws, index.txt kept
    {
        const fs::path root = freshTempDir("mgr_test_damaged");
        minigit::MiniGitRepository repo(root);
        repo.init();
        fs::remove(root / ".minigit" / "HEAD");

        if (repo.isInitialized()) return fail("damaged test failed: expected false", root);
        bool threw = false;
        try {
            repo.init();
        } catch (const std::runtime_error&) {
            threw = true;
        }
        if (!threw) return fail("damaged test failed: init did not throw", root);
        if (fs::exists(root / ".minigit" / "HEAD")) return fail("damaged test failed: init recreated HEAD", root);
        if (!fs::exists(root / ".minigit" / "index.txt")) return fail("damaged test failed: index.txt removed", root);
        fs::remove_all(root);
    }
    std::cout << "Damaged .minigit test passed\n";

    return 0;
}