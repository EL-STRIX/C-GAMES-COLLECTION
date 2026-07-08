# 🏗 Architecture & Design

This document details the internal architecture, design boundaries, and state-management protocols of the **C Games Collection**. It is intended for contributors maintaining or scaling the system.

## 1. Process Isolation

The repository embraces an **Event-Driven, Multi-Process Architecture** rather than compiling into a single monolithic binary. Each game, including the central launcher, is compiled as a completely independent, autonomous executable (`.exe`).

### Multi-Process Flow
When a user launches a game from the `launcher.exe` interface:
1. The launcher invokes GLib's `g_spawn_async` to execute the target game's binary natively on the host OS.
2. The launcher gracefully shuts down its GTK application loop (`gtk_window_close`).
3. The newly spawned game binary initializes its own GTK4 context, loads its memory structures, and takes over the screen.
4. When the user clicks "Return to Main Menu" from within a game, the game invokes the `return_to_launcher()` shared function (spawning the launcher back) and terminates itself.

**Why this design?**
By relying on the operating system to govern process lifecycles, memory leaks between sessions are mathematically impossible. State does not bleed from one game to the next.

```mermaid
graph TD
    L[Launcher.exe] -->|g_spawn_async| A[Game.exe]
    A -->|Initialize GTK Context| B[Main Window]
    B --> C[GtkStack]
    C --> D[Welcome Screen]
    C --> E[Game Screen]
    C --> F[Result Screen]
    
    U((User Input)) -->|Clicks/Keys| G[Signal Handlers]
    G --> H{AppData Struct Mutation}
    H -->|Update| E
    H -->|Game Over| F
    F -->|Return to Launcher| L
```

## 2. State Encapsulation (`AppData`)

In standard C GUI development, global variables are often lazily used to track widgets and state across callback scopes. This repository strictly prohibits global state variables.

Instead, every game module defines an `AppData` struct inside its `main.c`:

```c
// Example AppData Pattern
typedef struct {
    int current_score;
    char player_name[50];
    
    // UI References
    GtkWidget *window;
    GtkWidget *stack;
    GtkWidget *score_label;
} AppData;
```

During initialization (`activate`), `AppData` is allocated dynamically on the heap (`g_new0`), and a pointer to this context object is passed explicitly into every GTK signal handler as `user_data`. 
This guarantees thread-safe, scalable access to UI widgets and game properties without polluting the global namespace.

## 3. Data Persistence Engine

The project implements a centralized, zero-dependency data storage layer in `src/common/persistence.c`. 
It utilizes the battle-tested `GKeyFile` engine to parse and write standard INI configuration files to the disk (`data/`).

**Key features:**
- Provides atomic saves for high scores and theme settings.
- Features resilient GTK-compliant structured logging (`g_message`, `g_warning`) preventing silent application failure if IO is disrupted.
- Decouples file IO logic from UI threads.

## 4. UI Rendering & CSS Decoupling

GTK4 relies strictly on XML (`.ui` files) or programmatic C instantiation for the widget tree, and CSS for styling. 
This repository opts for **programmatic instantiation** to keep dependencies minimal, while styling is entirely decoupled into `.css` assets located in `assets/css/`.

When an application starts, `load_css_from_file()` builds an absolute path to the styling assets, dynamically streams the CSS into a `GtkCssProvider`, and binds it natively to the `GdkDisplay`. This achieves instant stylistic iteration without requiring full binary recompilations.
