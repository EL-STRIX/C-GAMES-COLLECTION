#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
// --- PERSISTENCE & THEME ENGINE INJECTED ---
#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>
#ifdef _WIN32
#include <windows.h>
#endif

int load_top_score(const char *game_name, char *out_player_name) {
    char filename[100];
    snprintf(filename, sizeof(filename), "%s_score.ini", game_name);
    
    GKeyFile *kf = g_key_file_new();
    if (!g_key_file_load_from_file(kf, filename, G_KEY_FILE_NONE, NULL)) {
        if (out_player_name) strcpy(out_player_name, "None");
        g_key_file_free(kf);
        return -1;
    }
    
    int score = g_key_file_get_integer(kf, "Score", "Value", NULL);
    gchar *name = g_key_file_get_string(kf, "Score", "Player", NULL);
    if (name && out_player_name) {
        strncpy(out_player_name, name, 49);
        out_player_name[49] = '\0';
    } else if (out_player_name) {
        strcpy(out_player_name, "Unknown");
    }
    
    g_free(name);
    g_key_file_free(kf);
    return score;
}

void save_score(const char *game_name, const char *player_name, int score, int is_lower_better) {
    char top_player[50]; 
    int top_score = load_top_score(game_name, top_player);
    int is_new_record = (top_score == -1) || (is_lower_better ? (score < top_score) : (score > top_score));
    
    if (is_new_record) {
        char filename[100]; 
        snprintf(filename, sizeof(filename), "%s_score.ini", game_name);
        GKeyFile *kf = g_key_file_new();
        g_key_file_set_string(kf, "Score", "Player", player_name);
        g_key_file_set_integer(kf, "Score", "Value", score);
        g_key_file_save_to_file(kf, filename, NULL);
        g_key_file_free(kf);
    }
}

void save_global_settings(const char *player_name, int theme_id) {
    GKeyFile *kf = g_key_file_new();
    g_key_file_set_string(kf, "Settings", "PlayerName", player_name);
    g_key_file_set_integer(kf, "Settings", "ThemeID", theme_id);
    g_key_file_save_to_file(kf, "settings.ini", NULL);
    g_key_file_free(kf);
}

void load_global_settings(char *player_name, int *theme_id) {
    GKeyFile *kf = g_key_file_new();
    if (g_key_file_load_from_file(kf, "settings.ini", G_KEY_FILE_NONE, NULL)) {
        gchar *name = g_key_file_get_string(kf, "Settings", "PlayerName", NULL);
        if (name) {
            strncpy(player_name, name, 49);
            player_name[49] = '\0';
            g_free(name);
        } else {
            strcpy(player_name, "Player 1");
        }
        
        GError *err = NULL;
        int t = g_key_file_get_integer(kf, "Settings", "ThemeID", &err);
        if (err) {
            *theme_id = 0;
            g_error_free(err);
        } else {
            *theme_id = t;
        }
    } else {
        strcpy(player_name, "Player 1");
        *theme_id = 0;
    }
    g_key_file_free(kf);
}

gboolean return_to_launcher(void) {
    char *full_path = NULL; 
    const char *exe_name = "launcher.exe";
#ifdef _WIN32
    char path[MAX_PATH]; GetModuleFileNameA(NULL, path, MAX_PATH);
    char *dir = g_path_get_dirname(path); full_path = g_build_filename(dir, exe_name, NULL); g_free(dir);
#else
    char *exe_path = g_file_read_link("/proc/self/exe", NULL);
    if (exe_path) {
        char *dir = g_path_get_dirname(exe_path); full_path = g_build_filename(dir, exe_name, NULL);
        g_free(dir); g_free(exe_path);
    } else { 
        char *cwd = g_get_current_dir();
        full_path = g_build_filename(cwd, exe_name, NULL); 
        g_free(cwd);
    }
#endif
    
    GError *error = NULL;
    if (!g_spawn_command_line_async(full_path, &error)) {
        GtkAlertDialog *dialog = gtk_alert_dialog_new("Failed to return to launcher: %s\nPath: %s", error->message, full_path);
        gtk_alert_dialog_show(dialog, NULL);
        g_object_unref(dialog);
        g_error_free(error);
        g_free(full_path);
        return FALSE;
    }
    g_free(full_path);
    return TRUE;
}

void apply_theme(int theme_id) {
    const char *theme_css = "";
    if (theme_id == 1) theme_css = "window { background-color: #11111b; } .card { background-color: #1e1e2e; color: #cdd6f4; border: 1px solid #45475a; box-shadow: 0 0 10px rgba(255,255,255,0.1); } label { color: #cdd6f4; }";
    else if (theme_id == 2) theme_css = "window { background-color: #0d0d0d; } .card { background-color: #000000; border: 2px solid #00ff00; box-shadow: 0 0 15px rgba(0,255,0,0.3); } label { color: #00ff00; font-family: monospace; } button { background-color: #002200; color: #00ff00; border: 1px solid #00ff00; }";
    if (theme_id != 0) {
        GtkCssProvider *provider = gtk_css_provider_new();
        gtk_css_provider_load_from_data(provider, theme_css, -1);
        gtk_style_context_add_provider_for_display(gdk_display_get_default(), GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
        g_object_unref(provider);
    }
}
// --- END INJECTED ENGINE ---

#ifdef _WIN32
#include <windows.h>
#endif

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
    ".btn-launch:active { background-color: #74c7ec; }"
    ".btn-settings { background-color: #313244; color: #cdd6f4; font-weight: bold; padding: 10px; border-radius: 8px; margin: 10px; }"
    ".btn-settings:hover { background-color: #45475a; }"
    ".btn-settings:active { background-color: #585b70; }"
    ".champions-panel { background-color: #181825; border-radius: 15px; padding: 20px; margin-top: 30px; border: 2px solid #313244; }"
    ".champ-title { font-size: 22px; font-weight: bold; color: #f9e2af; margin-bottom: 10px; }"
    ".champ-item { font-size: 16px; color: #cdd6f4; }";

GtkWidget *main_window = NULL;

static void on_save_settings(GtkButton *btn, gpointer user_data) {
    GtkWidget **widgets = (GtkWidget **)user_data;
    GtkEntryBuffer *buf = gtk_entry_get_buffer(GTK_ENTRY(widgets[0]));
    const char *name = gtk_entry_buffer_get_text(buf);
    int theme_id = gtk_drop_down_get_selected(GTK_DROP_DOWN(widgets[1]));
    
    save_global_settings(name, theme_id);
    gtk_window_destroy(GTK_WINDOW(widgets[2]));
    g_free(widgets);
    
    // Automatically apply theme to launcher if changed
    apply_theme(theme_id);
}

static void open_settings_dialog(GtkButton *btn, gpointer user_data) {
    GtkWindow *parent = GTK_WINDOW(user_data);
    GtkWidget *dialog = gtk_window_new();
    gtk_window_set_transient_for(GTK_WINDOW(dialog), parent);
    gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
    gtk_window_set_title(GTK_WINDOW(dialog), "Global Settings");
    gtk_window_set_default_size(GTK_WINDOW(dialog), 300, 200);
    
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_margin_start(box, 20); gtk_widget_set_margin_end(box, 20);
    gtk_widget_set_margin_top(box, 20); gtk_widget_set_margin_bottom(box, 20);
    
    char name[50];
    int theme_id;
    load_global_settings(name, &theme_id);
    
    GtkWidget *lbl_name = gtk_label_new("Global Player Name:");
    GtkWidget *entry_name = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(entry_name), name);
    
    GtkWidget *lbl_theme = gtk_label_new("Global Theme:");
    const char *themes[] = {"Default (Blue)", "Dark Mode", "Hacker", NULL};
    GtkWidget *dropdown = gtk_drop_down_new_from_strings(themes);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(dropdown), theme_id);
    
    GtkWidget *save_btn = gtk_button_new_with_label("Save & Close");
    gtk_widget_add_css_class(save_btn, "btn-launch");
    
    GtkWidget **widgets = g_new(GtkWidget*, 3);
    widgets[0] = entry_name;
    widgets[1] = dropdown;
    widgets[2] = dialog;
    g_signal_connect(save_btn, "clicked", G_CALLBACK(on_save_settings), widgets);
    
    gtk_box_append(GTK_BOX(box), lbl_name);
    gtk_box_append(GTK_BOX(box), entry_name);
    gtk_box_append(GTK_BOX(box), lbl_theme);
    gtk_box_append(GTK_BOX(box), dropdown);
    gtk_box_append(GTK_BOX(box), save_btn);
    
    gtk_window_set_child(GTK_WINDOW(dialog), box);
    gtk_window_present(GTK_WINDOW(dialog));
}

static void launch_game(GtkButton *btn, gpointer user_data)
{
    const char *exe_name = (const char *)user_data;
    char *full_path = NULL;

#ifdef _WIN32
    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH);
    char *dir = g_path_get_dirname(path);
    full_path = g_build_filename(dir, exe_name, NULL);
    g_free(dir);
#else
    char *exe_path = g_file_read_link("/proc/self/exe", NULL);
    if (exe_path) {
        char *dir = g_path_get_dirname(exe_path);
        full_path = g_build_filename(dir, exe_name, NULL);
        g_free(dir);
        g_free(exe_path);
    } else {
        char *cwd = g_get_current_dir();
        full_path = g_build_filename(cwd, exe_name, NULL);
        g_free(cwd);
    }
#endif

    GError *error = NULL;
    gboolean success = g_spawn_command_line_async(full_path, &error);
    if (!success) {
        GtkAlertDialog *dialog = gtk_alert_dialog_new("Failed to launch game: %s\nPath: %s", error->message, full_path);
        gtk_alert_dialog_show(dialog, NULL);
        g_object_unref(dialog);
        g_error_free(error);
    } else {
        if (main_window) {
            gtk_window_close(GTK_WINDOW(main_window));
        }
    }
    g_free(full_path);
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
    main_window = window;
    gtk_window_set_title(GTK_WINDOW(window), "C Games Collection - Launcher");
    gtk_window_set_default_size(GTK_WINDOW(window), 900, 600);
    
    // Apply Global Theme
    char dummy_name[50];
    int theme_id;
    load_global_settings(dummy_name, &theme_id);
    apply_theme(theme_id);
    
    GtkWidget *main_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_halign(main_vbox, GTK_ALIGN_CENTER);
    
    GtkWidget *header_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    
    GtkWidget *title = gtk_label_new("🎮 C GAMES COLLECTION");
    gtk_widget_add_css_class(title, "header-title");
    gtk_widget_set_hexpand(title, TRUE);
    
    GtkWidget *btn_settings = gtk_button_new_with_label("⚙️ Settings");
    gtk_widget_add_css_class(btn_settings, "btn-settings");
    gtk_widget_set_valign(btn_settings, GTK_ALIGN_CENTER);
    g_signal_connect(btn_settings, "clicked", G_CALLBACK(open_settings_dialog), window);
    
    gtk_box_append(GTK_BOX(header_box), title);
    gtk_box_append(GTK_BOX(header_box), btn_settings);
    
    GtkWidget *subtitle = gtk_label_new("Select a game from the library to begin your adventure.");
    gtk_widget_add_css_class(subtitle, "subtitle");
    
    gtk_box_append(GTK_BOX(main_vbox), header_box);
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
    
    // Champions Board
    GtkWidget *champ_frame = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_add_css_class(champ_frame, "champions-panel");
    gtk_widget_set_halign(champ_frame, GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(champ_frame, 500, -1);
    
    GtkWidget *champ_label = gtk_label_new("🏆 HALL OF FAME 🏆");
    gtk_widget_add_css_class(champ_label, "champ-title");
    gtk_box_append(GTK_BOX(champ_frame), champ_label);
    
    // Load top scores
    struct { const char *id; const char *name; const char *fmt; } games[] = {
        {"number_guessing", "Number Guessing", "%s: %s (%d guesses)"},
        {"rps", "Rock Paper Scissors", "%s: %s (%d wins)"},
        {"sgw", "Snake Gun Water", "%s: %s (%d wins)"},
        {"ttt_gui", "Epic Tic Tac Toe", "%s: %s (%d wins)"}
    };
    
    for (int i = 0; i < 4; i++) {
        char player[50];
        int score = load_top_score(games[i].id, player);
        if (score != -1) {
            char txt[100];
            snprintf(txt, sizeof(txt), games[i].fmt, games[i].name, player, score);
            GtkWidget *l = gtk_label_new(txt);
            gtk_widget_add_css_class(l, "champ-item");
            gtk_box_append(GTK_BOX(champ_frame), l);
        }
    }
    
    gtk_box_append(GTK_BOX(main_vbox), grid);
    gtk_box_append(GTK_BOX(main_vbox), champ_frame);
    
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
