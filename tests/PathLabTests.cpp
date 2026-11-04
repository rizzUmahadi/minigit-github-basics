#include "PathLab.hpp"
#include <iostream>
#include <chrono>
#include <string>
#include <fstream>

int main() {
    // Baseline smoke check: the project and CTest wiring work before students
    // begin the path Issues. Add focused assertions with each Issue.
    std::filesystem::path sample = "course/repo";
    if (sample.filename() != "repo") {
        std::cerr << "path smoke check failed\n";
        return 1;
    }
    std::cout << "PathLab smoke check passed\n";

    // Issue 1: ordinary root
    std::filesystem::path result = minigit::metadataDirectory("project");
    if (result != std::filesystem::path("project") / ".minigit") {
        std::cerr << "ordinary root test failed\n";
        return 1;
    }
    std::cout << "ordinary root test passed\n";

    // Issue 1: nested root
    std::filesystem::path nested = minigit::metadataDirectory("course/team/project");
    if (nested != std::filesystem::path("course") / "team" / "project" / ".minigit") {
        std::cerr << "nested root test failed\n";
        return 1;
    }
    std::cout << "nested root test passed\n";

    // Issue 2: ordinary root
    std::filesystem::path head = minigit::headFilePath("project");
    if (head != std::filesystem::path("project") / ".minigit" / "HEAD") {
        std::cerr << "ordinary HEAD path test failed\n";
        return 1;
    }
    std::cout << "ordinary HEAD path test passed\n";

    // Issue 2: nested root
    std::filesystem::path nestedHead = minigit::headFilePath("course/team/project");
    if (nestedHead != std::filesystem::path("course") / "team" / "project" / ".minigit" / "HEAD") {
        std::cerr << "nested HEAD path test failed\n";
        return 1;
    }
    std::cout << "nested HEAD path test passed\n";

    const auto diskRoot = std::filesystem::temp_directory_path() /
        ("minigit-head-path-test-" +
         std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    if (std::filesystem::exists(diskRoot)) {
        std::cerr << "disk side-effect test root already exists\n";
        return 1;
    }

    const auto diskHead = minigit::headFilePath(diskRoot);
    if (std::filesystem::exists(diskRoot) ||
        std::filesystem::exists(minigit::metadataDirectory(diskRoot)) ||
        std::filesystem::exists(diskHead)) {
        std::cerr << "HEAD path helper created a filesystem entry\n";
        return 1;
    }
    std::cout << "disk side-effect test passed\n";

    // Issue 3: writeText creates nested folders and writes the text
    std::filesystem::path testPath = std::filesystem::temp_directory_path() / "writeTextTest";
    std::filesystem::path testFile = testPath / "folder1" / "folder2" / "target.txt";
    std::string text = "Random test to be writen";
    if (!minigit::writeText(testFile, text)) {
        std::cerr << "writeText was unable to write into the file\n";
        std::filesystem::remove_all(testPath);
        return 1;
    }
    std::ifstream read(testFile);
    std::string readTest; 
    std::getline(read, readTest);
    read.close();
    std::filesystem::remove_all(testPath);
    if(readTest != text){
        std::cerr << "writeText text did not match testFile\n";
        return 1;
    }
    std::cout << "writeText nested temp directory test passed\n";

    // Issue 3: writeText returns false when the parent path is a regular file
    std::filesystem::path failRoot = std::filesystem::temp_directory_path() / "writeTextFailTest";
    std::filesystem::remove_all(failRoot);
    std::filesystem::create_directories(failRoot);

    std::filesystem::path blocker = failRoot / "blocker";
    {
        std::ofstream blockerFile(blocker);
        blockerFile << "I am a file, not a folder";
    }

    bool wrote = minigit::writeText(blocker / "x.txt", "should not be written");
    std::filesystem::remove_all(failRoot);
    if (wrote) {
        std::cerr << "writeText should return false when the parent is a regular file\n";
        return 1;
    }
    std::cout << "writeText failure test passed\n";

    return 0;
}