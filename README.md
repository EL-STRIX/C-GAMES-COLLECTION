# C Games Collection

[![CI Build](https://github.com/sujay/c-games-collection/actions/workflows/build.yml/badge.svg)](https://github.com/sujay/c-games-collection/actions)
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)

A curated collection of classic mini-games developed in C, featuring modern Graphical User Interfaces (GUIs) powered by GTK4. 

## Project Overview

The **C Games Collection** is a desktop application suite that demonstrates how to build interactive, state-driven software using the C programming language and the GTK4 toolkit. 

This repository serves as a reference implementation for:
- Managing GTK4 application lifecycles
- Event-driven programming in C
- Applying CSS styling to native C desktop applications
- Managing state using structs instead of global variables

## Features

### Games
* **Number Guessing Game**: Dynamic feedback, attempt tracking, and performance-based scoring.
* **Rock Paper Scissors**: 3-round battles against the computer with live score tracking.
* **Snake Gun Water**: A variation of RPS with unique emoji-based UI and logic.
* **Tic Tac Toe**: 2-player local multiplayer with dynamic grid updates and win detection.

### Core Architecture
* **Central Launcher**: A master menu to seamlessly navigate between games.
* **CSS Styling**: Styled components, cards, hover effects, and theme switching via GTK4 CSS providers.
* **Data Persistence**: Local INI file storage using GLib's `GKeyFile` for saving high scores and global player profiles.

## Technology Stack

| Layer | Technology |
| --- | --- |
| **Language** | C (C99/C11) |
| **GUI Toolkit** | GTK4 |
| **Data Persistence**| GLib (`GKeyFile`) |
| **Styling** | CSS |
| **Compiler** | GCC / Clang |
| **Build System** | GNU Make |

## Architecture

The project uses an event-driven, multi-process architecture. The central launcher asynchronously spawns game processes to isolate memory between applications.

For an in-depth breakdown of process boundaries, UI state management, and the persistence engine, see [ARCHITECTURE.md](ARCHITECTURE.md).

## Project Structure

```text
C-GAMES-COLLECTION/
├── src/
│   ├── common/             # Shared persistence and CSS engine
│   ├── launcher/           # Central Arcade Launcher
│   ├── number_guessing/    # Number Guessing logic and UI
│   ├── rock_paper_scissors/# Rock Paper Scissors logic and UI
│   ├── snake_gun_water/    # Snake Gun Water logic and UI
│   └── tic_tac_toe/        # Tic Tac Toe logic and UI
├── assets/
│   └── css/                # Isolated stylesheet themes
├── tests/
│   └── test_persistence.c  # GLib unit tests for the data layer
└── Makefile                # Automated build script
```

## Prerequisites

To compile and run from source, you need:

* **C Compiler**: `gcc` or `clang`
* **Make**: GNU Make or `mingw32-make`
* **GTK4 Development Libraries**: 
  * *Linux (Ubuntu/Debian)*: `sudo apt install libgtk-4-dev`
  * *Windows*: MSYS2 with `mingw-w64-x86_64-gtk4`
  * *macOS*: `brew install gtk4`
* **pkg-config**: For resolving library flags during compilation.

## Installation & Setup

1. **Clone the repository**
   ```bash
   git clone https://github.com/sujay/C-GAMES-COLLECTION.git
   cd C-GAMES-COLLECTION
   ```

2. **Compile the collection**
   ```bash
   make all
   ```
   *This generates the `bin/` directory and compiles the launcher and all games.*

## Running the Project

Execute the central launcher from the `bin` directory:

**Linux/macOS:**
```bash
./bin/launcher.exe
```

**Windows:**
```cmd
bin\launcher.exe
```

*Note: On Windows, ensure your `PATH` includes the GTK4 `bin` directories so dynamic libraries (`.dll`s) are located at runtime.*

## Testing

Run the automated headless unit tests for the persistence engine:
```bash
make test
```

## Contributing

Contributions are welcome. Please read our [Contributing Guidelines](.github/CONTRIBUTING.md) to understand our workflow and code style. Once your Pull Request is merged, you will be added to the [Contributors List](.github/CONTRIBUTORS.md).

## License

This project is licensed under the [MIT License](LICENSE).
