#include "FileSystem.hpp"

#include <chrono>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

int main()
{
    const auto testRoot = std::filesystem::temp_directory_path() /
                          ("minigit-write-bytes-test-" +
                           std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    const auto testFile = testRoot / "binary.dat";
    const std::vector<char> expected = {'M', '\0', static_cast<char>(0xFF), '\n', '\r', 'G'};
    minigit::FileSystem fileSystem;

    try
    {
        std::filesystem::create_directories(testRoot);
        fileSystem.writeBytes(testFile, expected);

        std::vector<char> actual;
        bool readFailed = false;
        {
            std::ifstream input(testFile, std::ios::binary);
            const std::istreambuf_iterator<char> begin(input);
            const std::istreambuf_iterator<char> end;
            actual.assign(begin, end);
            readFailed = input.bad();
        }
        if (readFailed || actual != expected)
        {
            std::cerr << "writeBytes did not preserve the exact binary contents\n";
            std::filesystem::remove_all(testRoot);
            return 1;
        }

        const auto blocker = testRoot / "not-a-directory";
        {
            std::ofstream blockerFile(blocker, std::ios::binary);
            blockerFile << "file";
        }

        bool threw = false;
        try
        {
            fileSystem.writeBytes(blocker / "target.bin", expected);
        }
        catch (const std::ios_base::failure &)
        {
            threw = true;
        }
        std::filesystem::remove_all(testRoot);

        if (!threw)
        {
            std::cerr << "writeBytes should throw when the destination cannot be opened\n";
            return 1;
        }
    }
    catch (const std::exception &error)
    {
        std::filesystem::remove_all(testRoot);
        std::cerr << "writeBytes test failed: " << error.what() << '\n';
        return 1;
    }

    std::cout << "writeBytes binary and failure tests passed\n";

        // FS-4: readBytes
    const auto readRoot = std::filesystem::temp_directory_path() /
        ("minigit-read-bytes-test-" +
         std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    try {
        std::filesystem::create_directories(readRoot);

        // Read a normal text file
        const auto textFile = readRoot / "text.txt";
        const std::string text = "hello\nworld";
        {
            std::ofstream out(textFile, std::ios::binary);
            out << text;
        }
        const std::vector<char> expectedText(text.begin(), text.end());
        if (fileSystem.readBytes(textFile) != expectedText) {
            std::cerr << "readBytes did not return the text file contents\n";
            std::filesystem::remove_all(readRoot);
            return 1;
        }

        // Read an empty file
        const auto emptyFile = readRoot / "empty.txt";
        {
            std::ofstream out(emptyFile, std::ios::binary);
        }
        if (!fileSystem.readBytes(emptyFile).empty()) {
            std::cerr << "readBytes should return an empty vector for an empty file\n";
            std::filesystem::remove_all(readRoot);
            return 1;
        }

        // A missing file should throw
        bool threw = false;
        try {
            fileSystem.readBytes(readRoot / "missing.txt");
        } catch (const std::ios_base::failure&) {
            threw = true;
        }
        if (!threw) {
            std::cerr << "readBytes should throw for a missing file\n";
            std::filesystem::remove_all(readRoot);
            return 1;
        }
    } catch (const std::exception& error) {
        std::filesystem::remove_all(readRoot);
        std::cerr << "readBytes test failed: " << error.what() << '\n';
        return 1;
    }
    std::filesystem::remove_all(readRoot);
    std::cout << "readBytes tests passed\n";

    // FS-3: fileExists
    const auto existsRoot = std::filesystem::temp_directory_path() /
        ("minigit-file-exists-test-" +
         std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    try {
        std::filesystem::create_directories(existsRoot);

        // An existing file returns true
        const auto realFile = existsRoot / "dummy.txt";
        {
            std::ofstream out(realFile, std::ios::binary);
            out << "hello";
        }
        if (!fileSystem.fileExists(realFile)) {
            std::cerr << "fileExists should return true for an existing file\n";
            std::filesystem::remove_all(existsRoot);
            return 1;
        }

        // A missing path returns false
        if (fileSystem.fileExists(existsRoot / "missing.txt")) {
            std::cerr << "fileExists should return false for a missing path\n";
            std::filesystem::remove_all(existsRoot);
            return 1;
        }

        // A folder is not a file, so it returns false
        if (fileSystem.fileExists(existsRoot)) {
            std::cerr << "fileExists should return false for a directory\n";
            std::filesystem::remove_all(existsRoot);
            return 1;
        }
    } catch (const std::exception& error) {
        std::filesystem::remove_all(existsRoot);
        std::cerr << "fileExists test failed: " << error.what() << '\n';
        return 1;
    }
    std::filesystem::remove_all(existsRoot);
    std::cout << "fileExists tests passed\n";
    // copy
    const auto copyRoot = std::filesystem::temp_directory_path() /
        ("minigit-copy-test-" +
         std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    try {
        std::filesystem::create_directories(copyRoot);

        // Copy a text file
        const auto textSource = copyRoot / "text-source.txt";
        const auto textTarget = copyRoot / "text-target.txt";
        const std::vector<char> textData = {'h', 'e', 'l', 'l', 'o', '\n', 'w', 'o', 'r', 'l', 'd'};
        fileSystem.writeBytes(textSource, textData);
        fileSystem.copy(textSource, textTarget);
        if (fileSystem.readBytes(textTarget) != textData) {
            std::cerr << "copy did not copy the text file contents\n";
            std::filesystem::remove_all(copyRoot);
            return 1;
        }
        if (fileSystem.readBytes(textSource) != textData) {
            std::cerr << "copy should leave the source file unchanged\n";
            std::filesystem::remove_all(copyRoot);
            return 1;
        }

        // Copy a file with binary data
        const auto binarySource = copyRoot / "binary-source.dat";
        const auto binaryTarget = copyRoot / "binary-target.dat";
        const std::vector<char> binaryData = {'M', '\0', static_cast<char>(0xFF), '\n', '\r', 'G'};
        fileSystem.writeBytes(binarySource, binaryData);
        fileSystem.copy(binarySource, binaryTarget);
        if (fileSystem.readBytes(binaryTarget) != binaryData) {
            std::cerr << "copy did not copy the binary file contents exactly\n";
            std::filesystem::remove_all(copyRoot);
            return 1;
        }

        // Copy over a target that already has longer content
        const auto overwriteSource = copyRoot / "overwrite-source.txt";
        const auto overwriteTarget = copyRoot / "overwrite-target.txt";
        const std::vector<char> shortData = {'n', 'e', 'w'};
        const std::vector<char> longData = {'o', 'l', 'd', ' ', 'c', 'o', 'n', 't', 'e', 'n', 't'};
        fileSystem.writeBytes(overwriteSource, shortData);
        fileSystem.writeBytes(overwriteTarget, longData);
        fileSystem.copy(overwriteSource, overwriteTarget);
        if (fileSystem.readBytes(overwriteTarget) != shortData) {
            std::cerr << "copy should fully overwrite an existing target with no leftover bytes\n";
            std::filesystem::remove_all(copyRoot);
            return 1;
        }

        // A missing source should throw and not create the target
        const auto missingSource = copyRoot / "missing-source.txt";
        const auto missingTarget = copyRoot / "missing-target.txt";
        bool threw = false;
        try {
            fileSystem.copy(missingSource, missingTarget);
        } catch (const std::ios_base::failure&) {
            threw = true;
        }
        if (!threw) {
            std::cerr << "copy should throw when the source does not exist\n";
            std::filesystem::remove_all(copyRoot);
            return 1;
        }
        if (std::filesystem::exists(missingTarget)) {
            std::cerr << "copy should not create the target when the source does not exist\n";
            std::filesystem::remove_all(copyRoot);
            return 1;
        }
    } catch (const std::exception& error) {
        std::filesystem::remove_all(copyRoot);
        std::cerr << "copy test failed: " << error.what() << '\n';
        return 1;
    }
    std::filesystem::remove_all(copyRoot);
    std::cout << "copy tests passed\n";

        // FS-5: createDirectories
    const auto dirRoot = std::filesystem::temp_directory_path() /
        ("minigit-create-dirs-test-" +
         std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    try {
        // Creates nested folders in one call
        const auto nested = dirRoot / "a" / "b" / "c";
        fileSystem.createDirectories(nested);
        if (!std::filesystem::is_directory(nested)) {
            std::cerr << "createDirectories did not create nested folders\n";
            std::filesystem::remove_all(dirRoot);
            return 1;
        }

        // Calling it again on an existing folder does nothing and doesn't throw
        fileSystem.createDirectories(nested);

        // A path below an existing file throws, and the file is left unchanged
        const auto blocker = dirRoot / "blocker.txt";
        {
            std::ofstream out(blocker, std::ios::binary);
            out << "keep me";
        }
        bool threw = false;
        try {
            fileSystem.createDirectories(blocker / "sub");
        } catch (const std::ios_base::failure&) {
            threw = true;
        }
        const std::vector<char> keepMe = {'k', 'e', 'e', 'p', ' ', 'm', 'e'};
        if (!threw || fileSystem.readBytes(blocker) != keepMe) {
            std::cerr << "createDirectories should throw and leave existing files alone\n";
            std::filesystem::remove_all(dirRoot);
            return 1;
        }
    } catch (const std::exception& error) {
        std::filesystem::remove_all(dirRoot);
        std::cerr << "createDirectories test failed: " << error.what() << '\n';
        return 1;
    }
    std::filesystem::remove_all(dirRoot);
    std::cout << "createDirectories tests passed\n";

       
    // FS-WRITEBYTES: empty data and unrelated files
    const auto emptyRoot = std::filesystem::temp_directory_path() /
        ("minigit-write-empty-test-" +
         std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    try {
        std::filesystem::create_directories(emptyRoot);

        // An unrelated file that must stay untouched
        const auto unrelated = emptyRoot / "unrelated.bin";
        const std::vector<char> unrelatedData = {'k', '\0', static_cast<char>(0xFF), '\r', '\n'};
        fileSystem.writeBytes(unrelated, unrelatedData);

        // Empty data creates a 0-byte file
        const auto emptyTarget = emptyRoot / "HEAD";
        fileSystem.writeBytes(emptyTarget, {});
        if (!fileSystem.fileExists(emptyTarget) || !fileSystem.readBytes(emptyTarget).empty()) {
            std::cerr << "writeBytes did not create an empty file for empty data\n";
            std::filesystem::remove_all(emptyRoot);
            return 1;
        }

        // Empty data replaces existing content completely
        const auto existing = emptyRoot / "index.txt";
        const std::vector<char> oldData = {'o', 'l', 'd', '\0', 'd', 'a', 't', 'a'};
        fileSystem.writeBytes(existing, oldData);
        fileSystem.writeBytes(existing, {});
        if (!fileSystem.readBytes(existing).empty()) {
            std::cerr << "writeBytes left old bytes behind when writing empty data\n";
            std::filesystem::remove_all(emptyRoot);
            return 1;
        }

        // Unrelated file is unchanged
        if (!fileSystem.fileExists(unrelated) || fileSystem.readBytes(unrelated) != unrelatedData) {
            std::cerr << "writeBytes changed or removed an unrelated file\n";
            std::filesystem::remove_all(emptyRoot);
            return 1;
        }
    } catch (const std::exception& error) {
        std::filesystem::remove_all(emptyRoot);
        std::cerr << "writeBytes empty data test failed: " << error.what() << '\n';
        return 1;
    }
    std::filesystem::remove_all(emptyRoot);
    std::cout << "writeBytes empty data tests passed\n"; 
   
    return 0;
}

