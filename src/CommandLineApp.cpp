#include "CommandLineApp.hpp"

#include <exception>
#include <iostream>
#include <string>
#include <utility>

namespace minigit {

CommandLineApp::CommandLineApp(const std::filesystem::path& root)
    : root_(root) {}

int CommandLineApp::run(int argc, char* argv[]) const {
    try {
        if (argc < 2) {
            std::cerr << "Error: no command given. Usage: minigit <command>\n";
            return 1;
        }

        const std::string command = argv[1];

        // TODO(#48): route "init" to MiniGitRepository::init() once it is merged.

        std::cerr << "Error: unknown command '" << command << "'\n";
        return 1;
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << "\n";
        return 1;
    }
}

} // namespace minigit