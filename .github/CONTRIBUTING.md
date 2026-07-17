# Contributing to C-GAMES-COLLECTION

Thank you for your interest in contributing. By participating, you agree to follow the architecture, coding conventions, and workflows defined below.

## 1. Getting Started

### Prerequisites
- **Compiler**: GCC or Clang (supporting C11).
- **Build System**: GNU Make (`make`) or Windows Batch (`build.bat`).
- **Dependencies**: GTK4 (`libgtk-4-dev` on Linux, `mingw-w64-ucrt-x86_64-gtk4` on MSYS2).

### Local Setup
Please refer to our comprehensive [Developer Setup Guide](../docs/DEVELOPMENT.md) for instructions on provisioning your environment (Windows, macOS, or Linux) and running the test suites.

Before writing code, please review the [Architecture Document](../docs/ARCHITECTURE.md) to understand the unified binary design and state encapsulation patterns.

If you are adding a new game, please follow the [Adding a New Game Tutorial](../docs/ADDING_A_GAME.md) step-by-step.

## 2. Coding Standards

We enforce strict coding conventions to ensure memory safety and maintainability in C11. 
All contributions **must** adhere to our [Style Guide](../docs/STYLE_GUIDE.md). This includes our zero-global-variable policy and naming conventions.

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
