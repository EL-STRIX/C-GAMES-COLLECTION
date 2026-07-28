# Developer Setup Guide

This guide details how to set up your local development environment to compile, debug, and test the **C Games Collection** across Windows, Linux, and macOS.

## 1. Prerequisites

To compile the codebase, you will need the following toolchain:
- A C11-compliant compiler (`gcc` or `clang`).
- `pkg-config` (for resolving GTK4 build flags).
- GNU Make.
- GTK4 Development Libraries.

## 2. Environment Setup

### Windows
Windows development requires MSYS2 to provide a Unix-like build environment and native Windows GTK4 libraries.
1. Download and install [MSYS2](https://www.msys2.org/).
2. Open the **MSYS2 UCRT64** terminal (do not use the MinGW64 or MSYS terminals).
3. Install the required toolchain:
   ```bash
   pacman -Syu
   pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-gtk4 mingw-w64-ucrt-x86_64-pkgconf make zip
   ```

### Linux (Ubuntu/Debian)
Linux natively supports GTK4 development. Install the required packages via `apt`:
```bash
sudo apt update
sudo apt install build-essential gcc make pkg-config libgtk-4-dev valgrind cppcheck
```

### macOS
For macOS, use Homebrew to install the toolchain:
```bash
brew update
brew install gcc make pkg-config gtk4
```

## 3. Building the Project

The build system relies on `make`.

- **Clean the build directory**:
  ```bash
  make clean
  ```
- **Build the unified binary**:
  ```bash
  make all
  ```

The resulting executable is generated in the `bin/` directory. Execute it directly:
```bash
# On Linux/macOS
./bin/c-games-collection.exe

# On Windows
bin\c-games-collection.exe
```

## 4. Local Testing & Verification

Before submitting a Pull Request, verify that your code passes the project's quality checks.

### Static Analysis
Run `cppcheck` to analyze the source code for memory safety issues, bugs, and portability problems:
```bash
cppcheck --enable=warning,performance,portability --suppress=missingIncludeSystem --error-exitcode=1 src/
```

### Memory Leak Testing (Linux Only)
Standard C requires manual memory management. Run Valgrind to verify that the non-GUI logic is free of memory leaks.
1. Build the test suite:
   ```bash
   make test
   ```
2. Run Valgrind on the compiled test binary:
   ```bash
   valgrind --leak-check=full --error-exitcode=1 ./bin/test_persistence.exe
   ```
If Valgrind detects memory leaks, the CI pipeline will fail.

## 5. UI Debugging with GTK Inspector

GTK4 includes a built-in inspector tool for debugging UI layouts and CSS styling.

To run the application with the inspector attached, prefix the execution command with `GTK_DEBUG`:
```bash
GTK_DEBUG=interactive ./bin/c-games-collection.exe
```
This tool allows developers to:
- Inspect the live widget hierarchy (e.g., `GtkStack`, `GtkBox`).
- Inject and modify CSS dynamically.
- Monitor signal emissions in real-time.
