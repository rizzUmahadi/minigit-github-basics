#include "PathLab.hpp"
#include <iostream>

namespace
{

    bool check(bool condition, const char *message)
    {
        if (!condition)
        {
            std::cerr << "FAILED: " << message << '\n';
            return false;
        }
        return true;
    }

} // namespace

int main()
{
    const std::filesystem::path ordinaryRoot = "course/repo";
    if (!check(minigit::metadataDirectory(ordinaryRoot) == ordinaryRoot / ".minigit",
               "metadataDirectory should append .minigit to an ordinary root"))
    {
        return 1;
    }

    const std::filesystem::path nestedRoot = "course/projects/repo";
    if (!check(minigit::metadataDirectory(nestedRoot) == nestedRoot / ".minigit",
               "metadataDirectory should append .minigit to a nested root"))
    {
        return 1;
    }

    std::cout << "PathLab tests passed\n";
    return 0;
}
// verifying metatdata existence