#include "PathLab.hpp"

namespace minigit {

std::filesystem::path metadata_dir(const std::filesystem::path& root) {
    return root / ".minigit";
}

} // namespace minigit
