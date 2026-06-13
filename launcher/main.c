#include <gtk/gtk.h>
#include <stdlib.h>

const char *css_data =
    "window { background-color: #1e1e2e; }"
    ".header-title { font-size: 32px; font-weight: 900; color: #cdd6f4; margin-top: 20px; margin-bottom: 10px; }"
    ".subtitle { font-size: 16px; color: #a6adc8; margin-bottom: 30px; }"
    ".game-card { background-color: #313244; border-radius: 15px; padding: 20px; margin: 10px; box-shadow: 0 4px 6px rgba(0,0,0,0.3); transition: all 0.2s ease; }"
    ".game-card:hover { background-color: #45475a; }"
    ".game-title { font-size: 20px; font-weight: bold; color: #89b4fa; }"
    ".game-desc { font-size: 14px; color: #bac2de; margin-top: 5px; }"
    ".btn-launch { background-color: #89b4fa; color: #11111b; font-weight: bold; padding: 10px 20px; border-radius: 8px; margin-top: 15px; }"
    ".btn-launch:hover { background-color: #b4befe; }"
    ".btn-launch:active { background-color: #74c7ec; }";

static void launch_game(GtkButton *btn, gpointer user_data)
{
    const char *exe_name = (const char *)user_data;
    
    // We try to launch it assuming it's in the same directory (Windows behavior)
    // or relative to the root if ran from the project root.
    GError *error = NULL;
    
    // First attempt: just the executable name (works on Windows if in same dir)
    gboolean success = g_spawn_command_line_async(exe_name, &error);
    if (!success) {
        g_print("Error launching %s: %s\n", exe_name, error->message);
        g_error_free(error);
        error = NULL;
        
        // Second attempt: prefix with ./bin/ (if ran from project root)
        char *path2 = g_strdup_printf("./bin/%s", exe_name);
        g_spawn_command_line_async(path2, NULL);
        g_free(path2);
    }
}

GtkWidget* create_game_entry(const char *icon, const char *title, const char *desc, const char *exe_name)
{
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_widget_add_css_class(box, "game-card");
    gtk_widget_set_size_request(box, 250, -1);
    
    GtkWidget *lbl_icon = gtk_label_new(icon);
    gtk_widget_set_margin_bottom(lbl_icon, 10);
    // Use large font for icon
    PangoAttrList *attrs = pango_attr_list_new();
    pango_attr_list_insert(attrs, pango_attr_scale_new(3.0));
    gtk_label_set_attributes(GTK_LABEL(lbl_icon), attrs);
    pango_attr_list_unref(attrs);
    
    GtkWidget *lbl_title = gtk_label_new(title);
    gtk_widget_add_css_class(lbl_title, "game-title");
    
    GtkWidget *lbl_desc = gtk_label_new(desc);
    gtk_widget_add_css_class(lbl_desc, "game-desc");
    gtk_label_set_wrap(GTK_LABEL(lbl_desc), TRUE);
    gtk_label_set_justify(GTK_LABEL(lbl_desc), GTK_JUSTIFY_CENTER);
    
    GtkWidget *btn = gtk_button_new_with_label("Play Now");
    gtk_widget_add_css_class(btn, "btn-launch");
    // Allocate string so it persists
    char *exe_str = g_strdup(exe_name);
    g_signal_connect(btn, "clicked", G_CALLBACK(launch_game), exe_str);
    
    gtk_box_append(GTK_BOX(box), lbl_icon);
    gtk_box_append(GTK_BOX(box), lbl_title);
    gtk_box_append(GTK_BOX(box), lbl_desc);
    gtk_box_append(GTK_BOX(box), btn);
    
    return box;
}

static void activate(GtkApplication *app, gpointer user_data)
{
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_string(provider, css_data);
    gtk_style_context_add_provider_for_display(
        gdk_display_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "C Games Collection - Launcher");
    gtk_window_set_default_size(GTK_WINDOW(window), 900, 600);
    
    GtkWidget *main_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_halign(main_vbox, GTK_ALIGN_CENTER);
    
    GtkWidget *title = gtk_label_new("🎮 C GAMES COLLECTION");
    gtk_widget_add_css_class(title, "header-title");
    
    GtkWidget *subtitle = gtk_label_new("Select a game from the library to begin your adventure.");
    gtk_widget_add_css_class(subtitle, "subtitle");
    
    gtk_box_append(GTK_BOX(main_vbox), title);
    gtk_box_append(GTK_BOX(main_vbox), subtitle);
    
    // Grid for games
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 20);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 20);
    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
    
    // Game 1
    GtkWidget *g1 = create_game_entry("🔢", "Number Guessing", "Read the computer's mind and guess the secret number.", "number-guessing.exe");
    gtk_grid_attach(GTK_GRID(grid), g1, 0, 0, 1, 1);
    
    // Game 2
    GtkWidget *g2 = create_game_entry("✊✋✌️", "Rock Paper Scissors", "The classic battle of wits against an AI opponent.", "rock-paper-scissors.exe");
    gtk_grid_attach(GTK_GRID(grid), g2, 1, 0, 1, 1);
    
    // Game 3
    GtkWidget *g3 = create_game_entry("🐍🔫💧", "Snake Gun Water", "A fun variation of RPS with new rules and emojis.", "snake-gun-water.exe");
    gtk_grid_attach(GTK_GRID(grid), g3, 2, 0, 1, 1);
    
    // Game 4
    GtkWidget *g4 = create_game_entry("⚔️", "Epic Tic Tac Toe", "An enhanced battle version of Tic Tac Toe.", "tic-tac-toe-gui.exe");
    gtk_grid_attach(GTK_GRID(grid), g4, 0, 1, 1, 1);
    
    gtk_box_append(GTK_BOX(main_vbox), grid);
    
    // Create a scrollable window just in case
    GtkWidget *scrolled = gtk_scrolled_window_new();
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled), main_vbox);
    gtk_window_set_child(GTK_WINDOW(window), scrolled);
    
    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char **argv)
{
    GtkApplication *app = gtk_application_new("org.sujay.gameslauncher", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}
