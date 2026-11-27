#include "CommandLineApp.hpp"
#include "MiniGitRepository.hpp"
#include <iostream>
#include <exception>
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

        if (command == "init") {
            MiniGitRepository repository(root_);
            const std::string metadataDir = (root_ / ".minigit").string();
            if (repository.isInitialized()) {
                // A valid repository is already here: say so and change nothing.
                std::cout << "MiniGit repository already exists in " << metadataDir
                          << " (nothing changed)\n";
                return 0;
            }
            // Empty folder: creates .minigit. Damaged .minigit: init() throws and
            // the catch below prints "Error: ..." and returns 1.
            repository.init();
            std::cout << "Initialized empty MiniGit repository in " << metadataDir << "\n";
            return 0;
        }

        std::cerr << "Error: unknown command '" << command << "'\n";
        return 1;
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << "\n";
        return 1;
    }
}

} // namespace minigit