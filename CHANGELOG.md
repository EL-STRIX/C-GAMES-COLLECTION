# Changelog

All notable changes to the **C-GAMES-COLLECTION** project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/), and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.0.0] - 2026-07-09

### Added
- Extracted all CSS themes into `assets/css/` to decouple styles from binaries.
- Introduced `load_css_from_file()` in the persistence engine to inject styles dynamically into the GTK4 application at runtime.
- Added structured GTK error logging (`g_warning`, `g_message`) across the `src/common/persistence.c` IO boundary to prevent silent failures.
- Created robust open-source documentation (`CONTRIBUTING.md`, `ARCHITECTURE.md`, `CODE_OF_CONDUCT.md`, `SECURITY.md`).

### Changed
- **Massive Architectural Refactor**: Restructured the root directory by renaming `games/` to `src/` and moving all independent binaries into strictly standardized `snake_case` directories (`number_guessing`, `rock_paper_scissors`, `snake_gun_water`, `tic_tac_toe`).
- Encapsulated massive global states in the Tic-Tac-Toe module into an isolated `AppData` struct to adhere to the project's strict anti-globals policy.
- Re-architected GTK callback signatures to correctly receive the local `AppData` state via `user_data` pointer handoffs.
- Enforced industry-standard code conventions (`UPPER_SNAKE_CASE` for constants, `snake_case` for variables).
- Re-wrote primitive code comments to explain "why" instead of "what".

### Removed
- Eliminated massive inline `css_data` C-strings that were cluttering UI source code.
- Purged dead variable declarations, duplicate `main_window` references in `launcher.c`, and unreferenced UI parameters across all modules.

## [0.1.0] - Initial Release

### Added
- Core Launcher interface.
- Number Guessing logic and UI.
- Rock Paper Scissors logic and UI.
- Snake Gun Water logic and UI.
- Tic Tac Toe local multiplayer UI.
- Basic GKeyFile data persistence module for storing INI configurations locally.
