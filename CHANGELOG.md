# Changelog

All notable changes to the **C-GAMES-COLLECTION** project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/), and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [5.0.0] - 2026-08-09

### Added
- Complete unit test suites for all four games: Tic Tac Toe, Rock Paper Scissors, Snake Gun Water, and Number Guessing.
- `tests/test_rock_paper_scissors.c`: Tests for RPS outcome logic, edge cases, and draw detection.
- `tests/test_snake_gun_water.c`: Full test suite for SGW win/loss/draw determination logic.
- `tests/test_number_guessing.c`: Tests for guess validation, hint generation, and scoring logic.
- `tests/test_tic_tac_toe.c`: Tests for win detection across all rows, columns, and diagonals.
- `test_plan.sh`: Shell script to orchestrate the full headless test execution plan.
- `Makefile` now compiles all five test binaries as part of `make all`.

### Changed
- `src/common/persistence.c`: Refactored to isolate file I/O behind a cleaner API; added path traversal sanitisation to protect against directory escape attacks.
- `src/common/persistence.h`: Extended with new safe path resolution declarations.
- `src/common/ui_utils.c`: Refactored CSS provider management to cache `GtkCssProvider` instances per stylesheet path, eliminating redundant disk I/O on repeated view transitions.
- `src/common/ui_utils.h`: Pruned unused exports; updated header to reflect the new caching API.
- `src/launcher/main.c`, `src/number_guessing/main.c`, `src/rock_paper_scissors/main.c`, `src/snake_gun_water/main.c`, `src/tic_tac_toe/main.c`: Updated call sites to use the refactored `ui_utils` and `persistence` APIs.
- `tests/test_persistence.c`: Refactored to execute inside a temporary sandbox directory to guarantee test isolation and prevent leftover state files.

### Fixed
- **Security**: Resolved a path traversal vulnerability in `persistence.c` where user-controlled input could navigate outside the application data directory using `../` sequences.
- **Performance**: CSS provider re-instantiation on every view transition has been eliminated; providers are now cached for the lifetime of the application.

---

## [1.0.0] - 2026-07-09

### Added
- Extracted CSS themes into `assets/css/` to decouple styles from binaries.
- Introduced `load_css_from_file()` in the persistence module to inject styles dynamically at runtime.
- Implemented structured GTK error logging (`g_warning`, `g_message`) across the `src/common/persistence.c` I/O boundary.
- Added standard open-source documentation (`CONTRIBUTING.md`, `ARCHITECTURE.md`, `CODE_OF_CONDUCT.md`, `SECURITY.md`).

### Changed
- Restructured the root directory by renaming `games/` to `src/` and organizing source files into module-specific directories.
- Refactored global state logic into isolated `AppData` structs to adhere to memory management constraints.
- Updated GTK callback signatures to reference local `AppData` state via the `user_data` pointer.
- Standardized codebase conventions (`UPPER_SNAKE_CASE` for constants, `snake_case` for variables).
- Revised code comments to focus strictly on architectural decisions and implementation logic.

### Removed
- Removed inline `css_data` C-strings from UI source files.
- Removed unused variable declarations, duplicate `main_window` references in `launcher.c`, and unreferenced UI parameters.

## [0.1.0] - Initial Release

### Added
- Core Launcher interface.
- Number Guessing logic and UI.
- Rock Paper Scissors logic and UI.
- Snake Gun Water logic and UI.
- Tic Tac Toe local multiplayer UI.
- Basic GKeyFile data persistence module for storing INI configurations locally.
