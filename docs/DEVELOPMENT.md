# Developer Setup Guide

This guide details how to set up your local development environment to compile, debug, and test the **C Games Collection** across different operating systems.

## 1. Prerequisites

Regardless of your operating system, you will need the following toolchain:
- A C11-compliant compiler (`gcc` or `clang`).
- `pkg-config` (for resolving GTK4 build flags).
- GNU Make.
- GTK4 Development Libraries.

## 2. Environment Setup

### Windows (Primary Target)
Windows development requires MSYS2 to provide a Unix-like build environment and native Windows GTK4 libraries.
1. Download and install [MSYS2](https://www.msys2.org/).
2. Open the **MSYS2 UCRT64** terminal (not the MinGW64 or MSYS terminals).
3. Install the required toolchain:
   ```bash
   pacman -Syu
   pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-gtk4 mingw-w64-ucrt-x86_64-pkgconf make zip
   ```

### Linux (Ubuntu/Debian)
Linux is the easiest environment to configure as GTK4 is natively supported.
```bash
sudo apt update
sudo apt install build-essential gcc make pkg-config libgtk-4-dev valgrind cppcheck
```

### macOS
For macOS, Homebrew is strictly recommended.
```bash
brew update
brew install gcc make pkg-config gtk4
```

## 3. Building the Project

The project is built entirely via `make`.

- **Clean the build directory**: `make clean`
- **Build the unified binary**: `make all`

The resulting executable will be placed in the `bin/` directory. You can run it directly:
```bash
# On Linux/macOS
./bin/c-games-collection.exe

# On Windows
bin\c-games-collection.exe
```

## 4. Local Testing & Verification

Before submitting a Pull Request, you must verify that your code adheres to our strict quality gates.

### Static Analysis
Run `cppcheck` to statically analyze the source code for memory safety issues and bugs:
```bash
cppcheck --enable=warning,performance,portability --suppress=missingIncludeSystem --error-exitcode=1 src/
```

### Memory Leak Testing (Linux Only)
Since standard C does not have a garbage collector, we rely on Valgrind to ensure our non-GUI logic doesn't leak memory.
1. Build the test suite: `make test`
2. Run Valgrind on the output:
```bash
valgrind --leak-check=full --error-exitcode=1 ./bin/test_persistence.exe
```
If Valgrind reports *any* definitively lost bytes, your build will fail in CI.

## 5. UI Debugging with GTK Inspector

If you are developing a new game or modifying CSS, you do not need to guess widget bounds. GTK4 includes a powerful built-in inspector.

To run the application with the inspector attached:
```bash
GTK_DEBUG=interactive ./bin/c-games-collection.exe
```
This tool allows you to:
- Inspect the live widget hierarchy (`GtkStack`, `GtkBox`, etc.).
- Dynamically inject and modify CSS on the fly.
- Monitor signal emissions in real-time.
