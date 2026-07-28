# Changelog

All notable changes to the **C-GAMES-COLLECTION** project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/), and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

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
