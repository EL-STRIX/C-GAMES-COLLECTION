# Contributing to C-GAMES-COLLECTION

Thank you for your interest in contributing. By participating, you agree to follow the architecture, coding conventions, and workflows defined below.

## 1. Getting Started

### Prerequisites
- **Compiler**: GCC or Clang (supporting C11).
- **Build System**: GNU Make (`make`) or Windows Batch (`build.bat`).
- **Dependencies**: GTK4 (`libgtk-4-dev` on Linux, `mingw-w64-ucrt-x86_64-gtk4` on MSYS2).

### Local Setup
1. Fork and clone the repository.
2. Run `make clean && make all` (or `build.bat`) to verify your environment builds cleanly.
3. Review `ARCHITECTURE.md` to understand the unified binary design and `AppData` encapsulation pattern.

## 2. Coding Standards

### C Style & Formatting
- **Naming Conventions**: Use `snake_case` for all files, directories, functions, and variables (e.g., `src/tic_tac_toe`, `process_round`).
- **Constants**: Use `UPPER_SNAKE_CASE` (e.g., `CHOICE_SNAKE`).
- **Structs**: Use `PascalCase` for typedefs (e.g., `AppData`, `GameState`).
- **State Management**: Do not use global variables for state or UI widgets. All state must be encapsulated within an `AppData` context object and passed down to GTK signal handlers via `user_data` pointers.
- **Linkage**: Define all internal module functions as `static` to prevent namespace pollution and compiler warnings.

### Comments & Documentation
- Comments should explain **why** a decision was made, not **what** the code is doing. Assume the reader understands standard C and GTK4.
- Use standard Doxygen notation (`/** ... */`) for public APIs in header files.

### Error Handling & Logging
- Handle errors explicitly.
- Use GTK's native structured logging (`g_warning`, `g_message`, `g_critical`) for persistence failures or I/O bounds.
- Always verify pointers returned by GTK builder functions or `g_new0` before accessing them.

## 3. Workflow & Collaboration Strategy

### Branch Naming Convention
We use a prefix-based branch naming strategy:
- `feat/` for new features or games (e.g., `feat/add-pong-game`)
- `fix/` for bug fixes (e.g., `fix/persistence-leak`)
- `docs/` for documentation improvements (e.g., `docs/update-readme`)
- `chore/` for build, CI, or tool changes (e.g., `chore/update-actions`)
- `refactor/` for structural code changes that do not alter behavior

### Commit Message Conventions
We follow [Conventional Commits](https://www.conventionalcommits.org/). Commit messages must be structured as follows:
`<type>[optional scope]: <description>`

Examples:
- `feat(persistence): add dynamic css loader`
- `fix(ttt): prevent diagonal win condition bug`
- `chore(ci): enforce make test on windows`
- `docs: update issue templates`

## 4. Pull Request Process

1. **Branching**: Checkout a new branch from `main` using the Branch Naming Convention above.
2. **Commit Messages**: Ensure all commits follow the Conventional Commits specification.
3. **Compilation**: Your code must compile cleanly without warnings (enforced by `-Wall -Wextra -Wpedantic -Wshadow -Wconversion`).
4. **Testing**: Run the test suite (`make test` or via `build.bat`) to ensure `persistence.c` boundaries remain intact. Run the GUI launcher to manually test UI transitions and asset loading.
5. **Review**: Submit a PR linking to the relevant issue.

## 5. Architectural Boundaries

Each game in `src/` is an independent module integrated into the unified `c-games-collection.exe` binary.
- Games may only share logic located in `src/common/`.
- Do not add hardcoded CSS directly to source files. Extract all styles to `assets/css/` and load them via `load_css_from_file()`.

If your change proposes a major architectural shift, open an issue labeled **[RFC] (Request for Comment)** first.
