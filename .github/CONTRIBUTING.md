# Contributing to C-GAMES-COLLECTION

Thank you for your interest in contributing to the **C Games Collection**. All contributors are expected to review and follow the architectural guidelines, coding conventions, and pull request workflows described below.

## 1. Getting Started

### Prerequisites
- **Compiler**: GCC or Clang (supporting C11).
- **Build System**: GNU Make (`make`) or Windows Batch (`build.bat`).
- **Dependencies**: GTK4 (`libgtk-4-dev` on Linux, `mingw-w64-ucrt-x86_64-gtk4` on MSYS2).

### Local Setup
Please refer to the [Developer Setup Guide](../docs/DEVELOPMENT.md) for instructions on provisioning your local environment (Windows, macOS, or Linux) and running the test suites.

Before writing code, review the [Architecture Document](../docs/ARCHITECTURE.md) to understand the unified binary design and the required state encapsulation patterns.

If you are adding a new game module, follow the [Adding a New Game Guide](../docs/ADDING_A_GAME.md).

## 2. Coding Standards

To ensure memory safety and maintainability in C11, all contributions must adhere to the [Style Guide](../docs/STYLE_GUIDE.md). This includes strict enforcement of a zero-global-variable policy and consistent naming conventions.

## 3. Workflow & Collaboration Strategy

### Branch Naming Convention
We use a prefix-based branch naming strategy:
- `feat/` for new features or game modules (e.g., `feat/add-pong-game`)
- `fix/` for bug fixes (e.g., `fix/persistence-leak`)
- `docs/` for documentation updates (e.g., `docs/update-readme`)
- `chore/` for build, CI, or toolchain changes (e.g., `chore/update-actions`)
- `refactor/` for structural changes that do not alter logic or behavior

### Commit Message Conventions
Commit messages must adhere to the [Conventional Commits](https://www.conventionalcommits.org/) specification:
`<type>[optional scope]: <description>`

Examples:
- `feat(persistence): add dynamic css loader`
- `fix(ttt): correct diagonal win condition logic`
- `chore(ci): enforce make test on windows pipeline`
- `docs: update issue templates`

## 4. Pull Request Process

1. **Branching**: Checkout a new branch from `main` using the Branch Naming Convention.
2. **Commit Messages**: Verify all commits follow the Conventional Commits specification.
3. **Compilation**: Ensure the code compiles without warnings. The build system enforces `-Wall -Wextra -Wpedantic -Wshadow -Wconversion`.
4. **Testing**: Run the unit test suite (`make test` or `build.bat`) to verify that the persistence boundaries and memory limits are intact. Launch the GUI to manually verify view transitions and asset loading.
5. **Review**: Submit a Pull Request and link it to the relevant issue.

## 5. Architectural Boundaries

Each game in `src/` functions as an independent module that compiles into the unified `c-games-collection.exe` binary.
- Game modules may only access shared logic located in `src/common/`.
- Do not add hardcoded CSS styling to C source files. Define styles in `assets/css/` and apply them dynamically via `load_css_from_file()`.

If a contribution proposes a significant architectural modification, open an issue labeled **[RFC] (Request for Comment)** prior to writing code.
