# Adding a New Game Module

This guide outlines the technical steps required to implement and integrate a new game into the **C Games Collection**.

The project uses a Unified Binary Architecture. Modules do not implement a standard `int main()` function. Instead, each game is compiled as a dependent module that integrates into the central launcher's `GtkStack`.

## 1. Directory Structure

Create a directory for the new game module inside `src/`. For example, to add "Pong":

```text
src/pong/
├── main.c
```
Additional `.c` and `.h` files can be added to this directory for more complex logic.

## 2. Defining the State (`AppData`)

The project enforces a strict no-global-variables policy to guarantee memory safety and prevent state contamination between modules. State and widget references must be encapsulated within an `AppData` struct.

In `src/pong/main.c`, define the module's state:

```c
#include <gtk/gtk.h>
#include "../common/persistence.h"
#include "../common/ui_utils.h"

typedef struct {
    int player_score;
    int opponent_score;
    gboolean is_playing;
    
    // UI References
    GtkWidget *score_label;
    GtkWidget *play_area;
} PongAppData;
```

## 3. Creating the UI Entry Point

Each module must expose a single public function to the launcher. This function must:
1. Allocate the `AppData` struct on the heap.
2. Programmatically construct the GTK widget tree.
3. Return the root `GtkWidget*`.

```c
// Implement back navigation
static void pong_on_back_clicked(GtkButton *btn, gpointer user_data) {
    // switch_to_launcher() is available globally from the launcher module
    extern void switch_to_launcher(void); 
    switch_to_launcher();
}

GtkWidget* pong_create_ui(void) {
    // 1. Allocate context on the heap using g_new0 to zero-initialize the memory
    PongAppData *app = g_new0(PongAppData, 1);
    app->player_score = 0;
    
    // 2. Build the root container
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    
    // 3. Construct UI components
    GtkWidget *title = gtk_label_new("Pong");
    gtk_widget_add_css_class(title, "title-large"); // Referencing global CSS classes
    
    GtkWidget *btn_back = gtk_button_new_with_label("Return to Menu");
    
    // 4. Connect signals, providing 'app' as the user_data context
    g_signal_connect(btn_back, "clicked", G_CALLBACK(pong_on_back_clicked), app);
    
    // 5. Assemble the widget tree
    gtk_box_append(GTK_BOX(vbox), title);
    gtk_box_append(GTK_BOX(vbox), btn_back);
    
    return vbox;
}
```

## 4. Registering the Game in the Launcher

After implementing the module's entry point, register it in the main application event loop.

Modify `src/launcher/main.c`:

1. Add the public function declaration:
   ```c
   extern GtkWidget* pong_create_ui(void);
   ```

2. Append the module to the stack logic within `launch_game()`:
   ```c
   // ... existing modules
   } else if (strcmp(game_id, "pong") == 0) {
       game_page = pong_create_ui();
   }
   ```

3. Construct the launcher entry card within `activate()`:
   ```c
   GtkWidget *g5 = create_game_entry("🏓", "Pong", "A classic arcade tennis game.", "pong");
   gtk_grid_attach(GTK_GRID(grid), g5, 0, 2, 1, 1); // Grid attachment: column 0, row 2
   ```

## 5. Updating the Build System

Modify the `Makefile` to include the new source file in the compilation target.

Append the source file to the `ALL_SRC` variable:
```makefile
ALL_SRC = src/launcher/main.c \
          src/number_guessing/main.c \
          src/rock_paper_scissors/main.c \
          src/snake_gun_water/main.c \
          src/tic_tac_toe/main.c \
          src/pong/main.c \  # <--- Append here
          src/common/persistence.c \
          src/common/ui_utils.c
```

*Note: If compiling natively on Windows via `build.bat`, update the GCC compilation command within that script as well.*

## 6. Compilation and Verification

Compile the project to test the integration:
```bash
make clean && make all
```

**Styling Requirement**: Do not hardcode CSS properties in the C source. Define new CSS classes in `assets/css/` and apply them using `gtk_widget_add_css_class(widget, "class-name")`.
