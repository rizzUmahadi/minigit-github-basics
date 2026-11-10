#include "RepositoryPaths.hpp"
#include <stdexcept>
#include <system_error>
namespace minigit {

RepositoryPaths::RepositoryPaths(std::filesystem::path projectRoot) {
    projectRoot_ = projectRoot;
    metadataRoot_ = projectRoot_ / ".minigit";
}

const std::filesystem::path& RepositoryPaths::projectRoot() const {
    return projectRoot_;
}

const std::filesystem::path& RepositoryPaths::metadataRoot() const {
    return metadataRoot_;
}

std::filesystem::path RepositoryPaths::headFile() const {
    return metadataRoot_ / "HEAD";
}

std::filesystem::path RepositoryPaths::indexFile() const {
    return metadataRoot_ / "index.txt";
}
std::filesystem::path RepositoryPaths::stageFile(const std::string& path) const {
    return metadataRoot_ / "stage" / path;
}
std::filesystem::path RepositoryPaths::commitDirectory(const std::string& id) const {
    return metadataRoot_ / "commits" / id;
}

std::filesystem::path RepositoryPaths::committedFile(const std::string& id, const std::string& path) const {
    return commitDirectory(id) / "files" / path;
}

std::string RepositoryPaths::safeRelativeFile(const std::string& argument) const {
    std::filesystem::path path(argument);
    if(argument.empty() || path.has_root_path()){
        throw std::runtime_error("invalid path, must be relative to the project");
    }
    path = path.lexically_normal();
    if (path.empty() || path == "." || *path.begin() == "..") {
        throw std::runtime_error("not a valid file in the project");
}
 for (const std::filesystem::path& part : path) {
    if (part == ".git" || part == ".minigit") {
        throw std::runtime_error("path points into repository data");
    }
}
        // PR-6: reject the path if any part that exists is a symbolic link
    std::filesystem::path current = projectRoot_;
    for (const std::filesystem::path& part : path) {
        current /= part;
        std::error_code error;
        if (std::filesystem::is_symlink(std::filesystem::symlink_status(current, error))) {
            throw std::runtime_error("path goes through a symbolic link");
        }
    }
    return path.generic_string();
}

} // namespace minigit