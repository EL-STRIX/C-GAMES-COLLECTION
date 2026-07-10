# C Games Collection

[![CI Build](https://github.com/EL-STRIX/C-GAMES-COLLECTION/actions/workflows/build.yml/badge.svg)](https://github.com/EL-STRIX/C-GAMES-COLLECTION/actions)
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)

A collection of classic mini-games developed in C, featuring native Graphical User Interfaces (GUIs) powered by GTK4. 

## Project Overview

The **C Games Collection** is a desktop application suite that demonstrates how to build interactive, state-driven software using the C programming language and the GTK4 toolkit. 

This repository serves as a reference implementation for:
- Managing GTK4 application lifecycles and view transitions (`GtkStack`).
- Event-driven programming and signal handling in C.
- Applying CSS styling to native C desktop applications.
- Managing application state securely using encapsulated structs instead of global variables.

## Features

### Games
* **Number Guessing Game**: Dynamic feedback, attempt tracking, and performance-based scoring.
* **Rock Paper Scissors**: 3-round series against the computer with live score tracking.
* **Snake Gun Water**: A variation of RPS with unique emoji-based UI and logic.
* **Tic Tac Toe**: 2-player local multiplayer with dynamic grid updates and win detection.

### Core Architecture
* **Central Launcher**: A unified application menu to navigate between games without spawning external processes.
* **CSS Styling**: Styled components, cards, hover effects, and theme switching via GTK4 CSS providers.
* **Data Persistence**: Local INI file storage using GLib's `GKeyFile` for saving high scores and global player profiles safely.

## Technology Stack

| Layer | Technology |
| --- | --- |
| **Language** | C (C11) |
| **GUI Toolkit** | GTK4 |
| **Data Persistence**| GLib (`GKeyFile`) |
| **Styling** | CSS |
| **Compiler** | GCC / Clang (strictly verified with `-Wall -Wextra -Wpedantic`) |
| **Build System** | GNU Make / Windows Batch |

## Architecture

The project uses a unified binary architecture driven by the GTK event loop. The central launcher manages a `GtkStack` to seamlessly transition between the main menu and individual game views while maintaining strict memory isolation via decoupled state structs.

For an in-depth breakdown of view management, UI state encapsulation, and the persistence engine, see [ARCHITECTURE.md](ARCHITECTURE.md).

## Project Structure

```text
C-GAMES-COLLECTION/
├── src/
│   ├── common/             # Shared persistence, UI utilities, and CSS engine
│   ├── launcher/           # Central Arcade Launcher and stack management
│   ├── number_guessing/    # Number Guessing logic and UI
│   ├── rock_paper_scissors/# Rock Paper Scissors logic and UI
│   ├── snake_gun_water/    # Snake Gun Water logic and UI
│   └── tic_tac_toe/        # Tic Tac Toe logic and UI
├── assets/
│   └── css/                # Isolated stylesheet themes
├── tests/
│   └── test_persistence.c  # GLib unit tests for the data layer
├── Makefile                # Automated GNU Make build script
└── build.bat               # Automated Windows build script
```

## Prerequisites

To compile and run from source, you need:

* **C Compiler**: `gcc` or `clang`
* **Make**: GNU Make or `mingw32-make`
* **GTK4 Development Libraries**: 
  * *Linux (Ubuntu/Debian)*: `sudo apt install libgtk-4-dev`
  * *Windows*: MSYS2 with `mingw-w64-ucrt-x86_64-gtk4`
  * *macOS*: `brew install gtk4`
* **pkg-config**: For resolving library flags during compilation.

## Installation & Setup

1. **Clone the repository**
   ```bash
   git clone https://github.com/EL-STRIX/C-GAMES-COLLECTION.git
   cd C-GAMES-COLLECTION
   ```

2. **Compile the collection**
   
   **Linux/macOS (Make):**
   ```bash
   make all
   ```
   
   **Windows (MSYS2 UCRT64):**
   ```cmd
   build.bat
   ```
   *This generates the `bin/` directory and compiles the unified binary.*

## Running the Application

Execute the application from the `bin` directory:

**Linux/macOS:**
```bash
./bin/c-games-collection.exe
```

**Windows:**
```cmd
bin\c-games-collection.exe
```

*Note: On Windows, ensure your `PATH` includes the GTK4 `bin` directories (e.g., `C:\msys64\ucrt64\bin`) so dynamic libraries (`.dll`s) are located at runtime.*

## Testing

Run the automated headless unit tests for the persistence engine:

**Linux/macOS:**
```bash
make test
```

**Windows:**
```cmd
build.bat
```
*(The batch script automatically builds and runs the test suite).*

## Contributing

Contributions are welcome. Please read our [Contributing Guidelines](.github/CONTRIBUTING.md) to understand our workflow, architecture constraints, and code style. Once your Pull Request is merged, you will be added to the [Contributors List](.github/CONTRIBUTORS.md).

## License

This project is licensed under the [MIT License](LICENSE).
