# Contributing to C-GAMES-COLLECTION

Thank you for your interest in contributing. This repository is built to professional engineering standards. By participating, you are expected to follow the architecture, coding conventions, and workflows defined below.

## 1. Getting Started

### Prerequisites
- **Compiler**: GCC or Clang (supporting C99/C11).
- **Build System**: GNU Make (`make` or `mingw32-make`).
- **Dependencies**: GTK4 (`libgtk-4-dev` or `mingw-w64-x86_64-gtk4`).

### Local Setup
1. Fork and clone the repository.
2. Run `make clean && make all` to verify your environment builds cleanly.
3. Check `ARCHITECTURE.md` to understand the multi-process design and `AppData` encapsulation pattern.

## 2. Coding Standards

### C Style & Formatting
- **Naming Conventions**: Use strict `snake_case` for all files, directories, functions, and variables (e.g., `src/tic_tac_toe`, `process_round`).
- **Constants**: Use `UPPER_SNAKE_CASE` (e.g., `CHOICE_SNAKE`).
- **Structs**: Use `PascalCase` for typedefs (e.g., `AppData`, `GameState`).
- **No Global Variables**: Under no circumstances should global variables be used for state or UI widgets. All state must be encapsulated within an `AppData` context object and passed down to GTK signal handlers via `user_data` pointers.

### Comments & Documentation
- Comments must explain **why** a decision was made, not **what** the code is doing. Assume the reader understands standard C and GTK4.
- Document cross-process boundaries (e.g., when invoking `g_spawn_async`) and memory allocation explicitly.

### Error Handling & Logging
- Do not fail silently.
- Use GTK's native structured logging (`g_warning`, `g_message`, `g_critical`) instead of primitive `printf` statements for persistence failures or IO bounds.
- Always check pointers returned by GTK builder functions or `g_new0` before accessing them.

## 3. Workflow & Collaboration Strategy

### Branch Naming Convention
We enforce a strict prefix-based branch naming strategy to keep workflows organized:
- `feat/` for new features or games (e.g., `feat/add-pong-game`)
- `fix/` for bug fixes (e.g., `fix/persistence-leak`)
- `docs/` for documentation improvements (e.g., `docs/update-readme`)
- `chore/` for build, CI, or tool changes (e.g., `chore/update-actions`)
- `refactor/` for structural code changes that do not alter behavior

### Commit Message Conventions
We strictly follow [Conventional Commits](https://www.conventionalcommits.org/). Commit messages must be structured as follows:
`<type>[optional scope]: <description>`

Examples:
- `feat(persistence): add dynamic css loader`
- `fix(ttt): prevent diagonal win condition bug`
- `chore(ci): enforce make test on windows`
- `docs: update issue templates`

### Versioning Strategy
This project strictly adheres to [Semantic Versioning (SemVer)](https://semver.org/).
- **MAJOR (`x.0.0`)**: Incompatible API changes, major architectural overhauls, or breaking persistence changes.
- **MINOR (`0.x.0`)**: Adding new games or features in a backward-compatible manner.
- **PATCH (`0.0.x`)**: Backward-compatible bug fixes, security patches, and documentation.
Releases are automatically tracked and tagged in the `CHANGELOG.md`.

## 4. Pull Request Process

1. **Branching**: Checkout a new branch from `main` using the Branch Naming Convention above.
2. **Commit Messages**: Ensure all commits follow the Conventional Commits specification.
3. **Compilation**: Your code must compile with absolutely zero warnings (`-Wall -Wextra`).
4. **Testing**: Run the test suite via `make test` to ensure `persistence.c` boundaries remain intact. Run the GUI launcher to manually test UI transitions and asset loading.
5. **Review**: Submit a PR linking to the relevant issue. Wait for maintainer approval and CI checks to pass before merging.

## 5. Architectural Boundaries

Before writing new games or shared modules, understand that `src/` isolates each game completely. 
- Games may only share logic located in `src/common/`.
- Do not add hardcoded CSS directly to source files. Extract all styles to `assets/css/` and load them via `load_css_from_file()`.

If your change proposes a major architectural shift, open an issue labeled **[RFC] (Request for Comment)** first.
