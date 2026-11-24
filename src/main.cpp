#include "CommandLineApp.hpp"

#include <filesystem>

int main(int argc, char* argv[]) {
    const minigit::CommandLineApp app(std::filesystem::current_path());
    return app.run(argc, argv);
}