# MiniGit Storage Format

This document describes the  layout of .minigit/, what each file and folder stores, and which commands create or modify each item.

## 1. `.minigit/` Directory & File Layout

When `minigit init` runs, MiniGit creates a `.minigit/` directory inside the project root (`<project-root>/.minigit`).

```text
<project-root>/
├── .minigit/
│   ├── HEAD
│   ├── index.txt
│   ├── stage/
│   └── commits/
└── <project files...>
