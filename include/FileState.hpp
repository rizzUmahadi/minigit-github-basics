#pragma once

#include <string>

namespace minigit {

// The six labels minigit status can show for one file.
enum class FileStatus {
    Untracked,
    Staged,
    StagedAndModified,
    Modified,
    Deleted,
    Clean
};

// Six yes/no observations about one file, turned into one status label.
// Needed in M5 (status).
struct FileState {
    bool tracked = false;                  // listed in index.txt
    bool staged = false;                   // has a copy in .minigit/stage
    bool inLatestCommit = false;           // saved in the commit HEAD points to
    bool existsInWorkingTree = false;      // still exists in the project folder
    bool workingDiffersFromStage = false;  // project copy differs from the staged copy
    bool stageDiffersFromCommit = false;   // staged copy differs from the committed copy

    FileStatus classify() const;
    std::string statusName(FileStatus status) const;
};

} // namespace minigit
