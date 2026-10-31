#include "PathLab.hpp"
#include <iostream>

namespace fs = std::filesystem;

static int failures = 0;

// Not assert(): CI builds in Release, where assert is compiled out.
#define CHECK(cond)                                                    \
    do {                                                               \
        if (!(cond)) {                                                 \
            std::cerr << "FAIL: " #cond " (line " << __LINE__ << ")\n"; \
            ++failures;                                                \
        }                                                              \
    } while (0)

int main() {
    // Baseline smoke check: the project and CTest wiring work.
    fs::path sample = "course/repo";
    CHECK(sample.filename() == "repo");

    // Ordinary root. Compare using fs::path so separators stay platform-neutral.
    CHECK(minigit::metadata_dir(fs::path("repo")) == fs::path("repo") / ".minigit");
    CHECK(minigit::metadata_dir(fs::path("repo")).filename() == ".minigit");

    // Nested root.
    fs::path nested = fs::path("course") / "week1" / "repo";
    CHECK(minigit::metadata_dir(nested) == nested / ".minigit");
    CHECK(minigit::metadata_dir(nested).parent_path() == nested);

    // Path-only: the helper must not create anything on disk.
    CHECK(!fs::exists(minigit::metadata_dir(fs::path("repo"))));
    CHECK(!fs::exists(minigit::metadata_dir(nested)));

    if (failures != 0) {
        return 1;
    }
    std::cout << "PathLab tests passed\n";
    return 0;
}
