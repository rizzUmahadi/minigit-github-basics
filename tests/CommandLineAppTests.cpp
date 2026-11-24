#include "CommandLineApp.hpp"

#include <iostream>
#include <sstream>
#include <string>

namespace {

// Runs the app with fake arguments and captures what it prints to std::cerr.
int runWithArgs(int argc, char* argv[], std::string& errorOutput) {
    std::ostringstream captured;
    std::streambuf* original = std::cerr.rdbuf(captured.rdbuf());

    const minigit::CommandLineApp app(".");
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
    // No command: just "minigit"
    {
        char program[] = "minigit";
        char* argv[] = {program};
        std::string errorOutput;
        if (runWithArgs(1, argv, errorOutput) != 1) {
            std::cerr << "no-command test failed: expected exit code 1\n";
            return 1;
        }
        if (!startsWithError(errorOutput)) {
            std::cerr << "no-command test failed: message must start with 'Error: '\n";
            return 1;
        }
    }
    std::cout << "No-command test passed\n";

    // Unknown command: "minigit status" (not built yet)
    {
        char program[] = "minigit";
        char command[] = "status";
        char* argv[] = {program, command};
        std::string errorOutput;
        if (runWithArgs(2, argv, errorOutput) != 1) {
            std::cerr << "unknown-command test failed: expected exit code 1\n";
            return 1;
        }
        if (!startsWithError(errorOutput)) {
            std::cerr << "unknown-command test failed: message must start with 'Error: '\n";
            return 1;
        }
    }
    std::cout << "Unknown-command test passed\n";

    return 0;
}