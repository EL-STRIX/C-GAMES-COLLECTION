# Coding Style Guide

To maintain a pristine, highly readable, and performant codebase, all contributions to the **C Games Collection** must strictly adhere to the following C11 and GTK4 coding standards.

## 1. Naming Conventions

Consistency in naming prevents cognitive overhead.

- **Files and Directories**: Strictly `snake_case`. (e.g., `src/tic_tac_toe`, `ui_utils.c`)
- **Functions and Variables**: Strictly `snake_case`. (e.g., `calculate_score()`, `current_player`)
- **Constants and Macros**: Strictly `UPPER_SNAKE_CASE`. (e.g., `MAX_SCORE`, `GRID_SIZE`)
- **Structs and Typedefs**: Strictly `PascalCase`. (e.g., `AppData`, `GameState`)

## 2. State Management & Memory

Because we are building a unified GUI application in C, memory leaks and global state contamination are our biggest threats.

### Zero Global Variables
You may **not** use global variables to track game state or store widget pointers. Doing so breaks the ability to reset the game cleanly and risks memory corruption between modules.

### The `AppData` Pattern
All state must be encapsulated within a context struct (`AppData`).
1. Dynamically allocate this struct when your game initializes using `g_new0(AppData, 1)`. (`g_new0` automatically zero-initializes the memory, preventing garbage data).
2. Pass this struct pointer as the `user_data` parameter to every `g_signal_connect` call.
3. Inside your callback, cast `user_data` back to your struct pointer to access your widgets and state.

```c
// DO THIS:
static void on_button_clicked(GtkWidget *widget, gpointer user_data) {
    MyAppData *app = (MyAppData *)user_data;
    app->score++;
    // update UI using app->score_label...
}
```

## 3. Scope and Linkage

C namespaces are global by default. To prevent function name collisions between different games (e.g., two games having an `update_ui` function), you must restrict the linkage of your functions.

- Every function that is not explicitly called by `src/launcher/main.c` **must** be declared `static`.

```c
// DO THIS:
static void reset_board(AppData *app) { ... }

// ONLY EXPOSE THE ENTRY POINT:
GtkWidget* mygame_create_ui(void) { ... }
```

## 4. GTK4 Specific Rules

- **Casting**: Use the official GTK casting macros (e.g., `GTK_WIDGET(obj)`, `GTK_LABEL(obj)`) rather than standard C casts `(GtkWidget*)obj`. The macros provide runtime type-checking in debug mode.
- **Styling**: Do not hardcode colors, margins, or fonts in C code using deprecated APIs. Instead, use `gtk_widget_add_css_class(widget, "class-name")` and define the styling in `assets/css/style.css`.
- **Error Handling**: Use GTK's native logging functions. Use `g_message()` for standard info, `g_warning()` for recoverable errors, and `g_critical()` or `g_error()` for fatal issues.

## 5. Commenting Philosophy

Assume the person reading your code understands C11 and GTK4. 
- Do **not** write comments that explain *what* the code is doing (e.g., `// Add 1 to score`).
- Do write comments that explain *why* a specific technical decision was made (e.g., `// We use g_idle_add here to prevent blocking the GTK render loop during file I/O`).
