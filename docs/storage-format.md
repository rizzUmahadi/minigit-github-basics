# MiniGit Storage Format

This document describes the layout of `.minigit/`, what each file and folder stores, and which commands create or modify each item.

## 1. `.minigit/` Directory & File Layout

When `minigit init` runs in a fresh directory, MiniGit creates a `.minigit/` directory inside the project root (`<project-root>/.minigit`):

```text
<project-root>/
├── .minigit/
│   ├── HEAD
│   ├── index.txt
│   ├── stage/
│   └── commits/
└── <project files...>
```

## 2. Contents & Command Responsibilities

| Path inside `.minigit/` | What It Holds | Created / Changed By |
| :--- | :--- | :--- |
| `.minigit/` | Metadata root directory for the repository (`metadataRoot()`). Checked by `isInitialized()` to verify whether the repository exists. | Created by `init`; skipped by `WorkingTree` when scanning project files. |
| `HEAD` | Stores the reference/ID of the latest commit. Initialized as an empty (0-byte) file before the first commit. | Created as an empty file by `init`; updated after a commit completes (`commit` / `HeadRef`). |
| `index.txt` | Lists tracked/staged file paths (one relative path per line). Initialized as an empty (0-byte) file with 0 tracked paths. | Created as an empty file by `init`; updated when staging (`add`) or committing (`commit` / `IndexTable`). |
| `stage/` | Staging area holding copies of staged files prior to committing. | Created empty by `init`; populated by `add` and cleared/updated during `commit`. |
| `commits/` | Stores saved commit snapshots and metadata. | Created empty by `init`; written to by `commit` and read by `log` / `status`. |

## 3. What `minigit init` does in each case

| Case | Output | Exit code | Changes on disk |
| :--- | :--- | :--- | :--- |
| Empty folder (no `.minigit/`) | `Initialized empty MiniGit repository in <root>/.minigit` | 0 | Creates `.minigit/`, `stage/`, `commits/`, and empty `HEAD` and `index.txt` |
| Valid repository already there (`.minigit/`, `HEAD` and `index.txt` all present) | `MiniGit repository already exists in <root>/.minigit (nothing changed)` | 0 | None |
| Damaged `.minigit/` (exists, but `HEAD` or `index.txt` is missing) | `Error: damaged repository: ...` | 1 | None. MiniGit never overwrites or repairs existing repository data |

`MiniGitRepository::isInitialized()` is true only in the "valid repository" case. `MiniGitRepository::init()` throws `std::runtime_error` whenever `.minigit/` already exists, so it can never overwrite data. `CommandLineApp::run()` checks `isInitialized()` first to print the friendly message, and turns any exception into `Error: <message>` with exit code 1.

Any other command (for example `minigit status`) prints `Error: unknown command ...` and exits with 1 until its milestone is built. Running `minigit` with no command prints `Error: no command given ...` and exits with 1.
