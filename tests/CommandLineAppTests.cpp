#include "CommandLineApp.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace fs = std::filesystem;

namespace {

// Runs the app in `root` with fake arguments and captures what it prints to std::cerr.
int runWithArgs(const fs::path& root, int argc, char* argv[], std::string& errorOutput) {
    std::ostringstream captured;
    std::streambuf* original = std::cerr.rdbuf(captured.rdbuf());

    const minigit::CommandLineApp app(root);
    const int exitCode = app.run(argc, argv);

    std::cerr.rdbuf(original);
    errorOutput = captured.str();
    return exitCode;
}

bool startsWithError(const std::string& text) {
    return text.rfind("Error: ", 0) == 0;
}

} // namespace

int main() {
    char program[] = "minigit";

    // No command: just "minigit"
    {
        char* argv[] = {program};
        std::string errorOutput;
        if (runWithArgs(".", 1, argv, errorOutput) != 1 || !startsWithError(errorOutput)) {
            std::cerr << "no-command test failed\n";
            return 1;
        }
    }
    std::cout << "No-command test passed\n";

    // Unknown command: "minigit status" (not built yet)
    {
        char command[] = "status";
        char* argv[] = {program, command};
        std::string errorOutput;
        if (runWithArgs(".", 2, argv, errorOutput) != 1 || !startsWithError(errorOutput)) {
            std::cerr << "unknown-command test failed\n";
            return 1;
        }
    }
    std::cout << "Unknown-command test passed\n";

    // init in an empty folder, then again on a valid repository, then on a damaged one
    {
        const fs::path root = fs::temp_directory_path() /
            ("cla_test_init_" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
        fs::remove_all(root);
        fs::create_directories(root);

        char command[] = "init";
        char* argv[] = {program, command};
        std::string errorOutput;

        // Empty folder: returns 0 and creates the layout
        if (runWithArgs(root, 2, argv, errorOutput) != 0 || !errorOutput.empty()) {
            std::cerr << "init test failed: first init did not return 0 cleanly\n";
            fs::remove_all(root);
            return 1;
        }
        if (!fs::is_directory(root / ".minigit" / "stage") ||
            !fs::is_directory(root / ".minigit" / "commits") ||
            !fs::exists(root / ".minigit" / "HEAD") ||
            !fs::exists(root / ".minigit" / "index.txt")) {
            std::cerr << "init test failed: .minigit layout was not created\n";
            fs::remove_all(root);
            return 1;
        }

        // Valid repository already there: prints a message, returns 0, changes nothing
        {
            std::ofstream head(root / ".minigit" / "HEAD", std::ios::binary);
            head << "1";
        }
        if (runWithArgs(root, 2, argv, errorOutput) != 0 || !errorOutput.empty()) {
            std::cerr << "init test failed: init on a valid repository should return 0 without an error\n";
            fs::remove_all(root);
            return 1;
        }
        if (fs::file_size(root / ".minigit" / "HEAD") != 1) {
            std::cerr << "init test failed: init on a valid repository changed HEAD\n";
            fs::remove_all(root);
            return 1;
        }

        // Damaged .minigit (HEAD missing): prints Error:, returns 1, does not recreate HEAD
        fs::remove(root / ".minigit" / "HEAD");
        if (runWithArgs(root, 2, argv, errorOutput) != 1 || !startsWithError(errorOutput)) {
            std::cerr << "init test failed: damaged .minigit should print Error: and return 1\n";
            fs::remove_all(root);
            return 1;
        }
        if (fs::exists(root / ".minigit" / "HEAD")) {
            std::cerr << "init test failed: init recreated HEAD in a damaged .minigit\n";
            fs::remove_all(root);
            return 1;
        }
        fs::remove_all(root);
    }
    std::cout << "Init command test passed\n";

    return 0;
}