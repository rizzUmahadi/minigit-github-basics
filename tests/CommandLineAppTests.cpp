#include "CommandLineApp.hpp"

#include <filesystem>
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

    // init: first run succeeds, second run fails with Error:
    {
        const fs::path root = fs::temp_directory_path() / "cla_test_init";
        fs::remove_all(root);
        fs::create_directories(root);

        char command[] = "init";
        char* argv[] = {program, command};
        std::string errorOutput;

        if (runWithArgs(root, 2, argv, errorOutput) != 0) {
            std::cerr << "init test failed: first init did not return 0\n";
            fs::remove_all(root);
            return 1;
        }
        if (!fs::is_directory(root / ".minigit")) {
            std::cerr << "init test failed: .minigit was not created\n";
            fs::remove_all(root);
            return 1;
        }
        if (runWithArgs(root, 2, argv, errorOutput) != 1 || !startsWithError(errorOutput)) {
            std::cerr << "init test failed: second init should print Error: and return 1\n";
            fs::remove_all(root);
            return 1;
        }
        fs::remove_all(root);
    }
    std::cout << "Init command test passed\n";

    return 0;
}