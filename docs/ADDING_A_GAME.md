# Adding a New Game

This tutorial provides a step-by-step technical guide for contributors looking to build and integrate a new game into the **C Games Collection**.

Because we use a Unified Binary Architecture, you will not be writing a standard `int main()` function. Instead, your game will exist as an independent module that hooks into the central launcher's `GtkStack`.

## 1. Directory Structure

First, create a new directory for your game inside `src/`. For example, if you are creating Pong:

```text
src/pong/
├── main.c
```
*(You may add additional `.c` and `.h` files to this directory if your game is complex, but a single `main.c` is fine for simple games).*

## 2. Defining the `AppData` Struct

To ensure memory safety and prevent cross-module contamination, **global variables are strictly prohibited**. Instead, you must encapsulate your game's state and widget references inside an `AppData` struct.

In `src/pong/main.c`, define your state:

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

Your module must expose exactly one public function to the central launcher. This function must allocate your `AppData` struct on the heap, construct the GTK widget tree programmatically, and return the root `GtkWidget*`.

```c
// Define standard back navigation
static void pong_on_back_clicked(GtkButton *btn, gpointer user_data) {
    // switch_to_launcher() is globally available from the launcher
    extern void switch_to_launcher(void); 
    switch_to_launcher();
}

GtkWidget* pong_create_ui(void) {
    // 1. Allocate context on the heap (zero-initialized)
    PongAppData *app = g_new0(PongAppData, 1);
    app->player_score = 0;
    
    // 2. Build the root container (usually a GtkBox)
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    
    // 3. Build UI components
    GtkWidget *title = gtk_label_new("Pong");
    gtk_widget_add_css_class(title, "title-large"); // Using global CSS classes
    
    GtkWidget *btn_back = gtk_button_new_with_label("Return to Menu");
    
    // 4. Connect signals, passing 'app' as user_data
    g_signal_connect(btn_back, "clicked", G_CALLBACK(pong_on_back_clicked), app);
    
    // 5. Assemble tree
    gtk_box_append(GTK_BOX(vbox), title);
    gtk_box_append(GTK_BOX(vbox), btn_back);
    
    return vbox;
}
```

## 4. Registering the Game in the Launcher

Now that your module can generate its UI, you must hook it into the main application loop.

Open `src/launcher/main.c`:

1. Add your public header/function declaration at the top:
   ```c
   extern GtkWidget* pong_create_ui(void);
   ```

2. Inside `launch_game()`, add your game to the stack logic:
   ```c
   // ... existing games
   } else if (strcmp(game_id, "pong") == 0) {
       game_page = pong_create_ui();
   }
   ```

3. Inside `activate()`, create the launcher card for your game so the user can click it:
   ```c
   GtkWidget *g5 = create_game_entry("🏓", "Pong", "A classic arcade tennis game.", "pong");
   gtk_grid_attach(GTK_GRID(grid), g5, 0, 2, 1, 1); // Attach to column 0, row 2
   ```

## 5. Updating the Build System

To compile your new module into the unified binary, you must update the `Makefile`.

Open `Makefile` and append your source file to `ALL_SRC`:
```makefile
ALL_SRC = src/launcher/main.c \
          src/number_guessing/main.c \
          src/rock_paper_scissors/main.c \
          src/snake_gun_water/main.c \
          src/tic_tac_toe/main.c \
          src/pong/main.c \  # <--- Add your file here
          src/common/persistence.c \
          src/common/ui_utils.c
```

*(Note: If you are building on Windows natively using the legacy `build.bat`, ensure you update the `gcc` command inside that file as well).*

## 6. Compile and Test

Run `make clean && make all` and test your game. If you need to style your widgets, do not hardcode CSS. Instead, add new classes to `assets/css/` and apply them using `gtk_widget_add_css_class(widget, "your-class")`.
