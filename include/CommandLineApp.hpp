#pragma once

#include <filesystem>

namespace minigit {

class CommandLineApp {
public:
    explicit CommandLineApp(const std::filesystem::path& root);
    int run(int argc, char* argv[]) const;

private:
    std::filesystem::path root_;
};

} // namespace minigit