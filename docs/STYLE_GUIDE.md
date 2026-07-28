# Coding Style Guide

To maintain a maintainable codebase, contributions to the **C Games Collection** must adhere to the following C11 and GTK4 coding standards.

## 1. Naming Conventions

Consistent naming conventions reduce cognitive overhead for developers reading the code.

- **Files and Directories**: Use `snake_case`. (e.g., `src/tic_tac_toe`, `ui_utils.c`)
- **Functions and Variables**: Use `snake_case`. (e.g., `calculate_score()`, `current_player`)
- **Constants and Macros**: Use `UPPER_SNAKE_CASE`. (e.g., `MAX_SCORE`, `GRID_SIZE`)
- **Structs and Typedefs**: Use `PascalCase`. (e.g., `AppData`, `GameState`)

## 2. State Management & Memory

In a unified GUI application written in C, preventing memory leaks and global state contamination is a priority.

### Zero Global Variables
Do not use global variables to track game state or store widget pointers. Relying on global state prevents resetting a game cleanly and increases the risk of memory corruption between modules.

### The `AppData` Pattern
Encapsulate all state within a context struct (`AppData`).
1. Dynamically allocate this struct during game initialization using `g_new0(AppData, 1)`. The `g_new0` function zero-initializes the memory, mitigating undefined behavior from uninitialized data.
2. Pass this struct pointer as the `user_data` parameter to every `g_signal_connect` call.
3. Inside your callback, cast `user_data` back to your struct pointer to access required widgets and state.

```c
// Recommended pattern:
static void on_button_clicked(GtkWidget *widget, gpointer user_data) {
    MyAppData *app = (MyAppData *)user_data;
    app->score++;
    // update UI using app->score_label
}
```

## 3. Scope and Linkage

C namespaces are global by default. To prevent function name collisions between different modules (e.g., multiple games implementing an `update_ui` function), restrict the linkage of your functions.

- Declare every function that is not explicitly called by `src/launcher/main.c` as `static`.

```c
// Recommended pattern:
static void reset_board(AppData *app) { ... }

// Only expose the entry point to the global namespace:
GtkWidget* mygame_create_ui(void) { ... }
```

## 4. GTK4 Specific Rules

- **Casting**: Use the official GTK casting macros (e.g., `GTK_WIDGET(obj)`, `GTK_LABEL(obj)`) rather than standard C casts `(GtkWidget*)obj`. These macros provide runtime type-checking when debugging.
- **Styling**: Do not hardcode colors, margins, or fonts in C code. Instead, use `gtk_widget_add_css_class(widget, "class-name")` and define the relevant styling in `assets/css/style.css`.
- **Error Handling**: Use GTK's native logging functions. Use `g_message()` for informational output, `g_warning()` for recoverable errors, and `g_critical()` or `g_error()` for fatal issues.

## 5. Commenting Philosophy

Assume the reader understands C11 and GTK4.
- Avoid comments that explain *what* the code is doing (e.g., `// Add 1 to score`).
- Write comments that explain *why* a specific technical decision was made (e.g., `// We use g_idle_add here to prevent blocking the GTK render loop during file I/O`).
