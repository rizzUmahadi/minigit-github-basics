#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>

#ifndef _WIN32
#include <sys/wait.h>
#endif

namespace fs = std::filesystem;

namespace {

std::string minigitPath;

// Runs the real minigit program inside `dir` and returns its exit code.
// stdout and stderr are saved to out.txt and err.txt in that folder.
int runMinigit(const fs::path& dir, const std::string& arguments) {
    const fs::path previous = fs::current_path();
    fs::current_path(dir);
    std::string command = "\"" + minigitPath + "\"";
    if (!arguments.empty()) {
        command += " " + arguments;
    }
    command += " > out.txt 2> err.txt";
    const int status = std::system(command.c_str());
    fs::current_path(previous);
#ifdef _WIN32
    return status;
#else
    return WIFEXITED(status) ? WEXITSTATUS(status) : -1;
#endif
}

std::string readFile(const fs::path& path) {
    std::ifstream file(path, std::ios::binary);
    return std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
}

bool printedError(const fs::path& dir) {
    return readFile(dir / "err.txt").rfind("Error: ", 0) == 0;
}

fs::path freshDir(const char* name) {
    const fs::path dir = fs::temp_directory_path() / name;
    fs::remove_all(dir);
    fs::create_directories(dir);
    return dir;
}

int fail(const std::string& message, const fs::path& dir) {
    std::cerr << message << "\n";
    fs::remove_all(dir);
    return 1;
}

} // namespace

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "usage: minigit_init_integration_test <path-to-minigit>\n";
        return 1;
    }
    minigitPath = fs::absolute(argv[1]).string();

    // Empty folder: init creates the full layout and exits 0
    const fs::path root = freshDir("minigit_integration_init");
    const fs::path meta = root / ".minigit";
    if (runMinigit(root, "init") != 0) return fail("first init did not exit 0", root);
    if (!fs::is_directory(meta)) return fail(".minigit missing", root);
    if (!fs::is_directory(meta / "stage")) return fail("stage/ missing", root);
    if (!fs::is_directory(meta / "commits")) return fail("commits/ missing", root);
    if (!fs::is_regular_file(meta / "HEAD") || fs::file_size(meta / "HEAD") != 0)
        return fail("HEAD missing or not empty", root);
    if (!fs::is_regular_file(meta / "index.txt") || fs::file_size(meta / "index.txt") != 0)
        return fail("index.txt missing or not empty", root);
    std::cout << "Init in empty folder passed\n";

    // Valid repository already there: Error:, exit 1, nothing changed
    {
        std::ofstream(meta / "HEAD", std::ios::binary) << "001";
    }
    if (runMinigit(root, "init") != 1) return fail("second init did not exit 1", root);
    if (!printedError(root)) return fail("second init did not print Error:", root);
    if (readFile(meta / "HEAD") != "001") return fail("second init changed HEAD", root);
    std::cout << "Init twice passed\n";

    // Damaged .minigit (HEAD missing): Error:, exit 1, HEAD not recreated
    fs::remove(meta / "HEAD");
    if (runMinigit(root, "init") != 1) return fail("damaged init did not exit 1", root);
    if (!printedError(root)) return fail("damaged init did not print Error:", root);
    if (fs::exists(meta / "HEAD")) return fail("damaged init recreated HEAD", root);
    std::cout << "Damaged .minigit passed\n";
    fs::remove_all(root);

    // Unknown command: Error:, exit 1
    const fs::path other = freshDir("minigit_integration_other");
    if (runMinigit(other, "status") != 1) return fail("unknown command did not exit 1", other);
    if (!printedError(other)) return fail("unknown command did not print Error:", other);
    std::cout << "Unknown command passed\n";

    // No command: Error:, exit 1
    if (runMinigit(other, "") != 1) return fail("no command did not exit 1", other);
    if (!printedError(other)) return fail("no command did not print Error:", other);
    std::cout << "No command passed\n";
    fs::remove_all(other);

    return 0;
}