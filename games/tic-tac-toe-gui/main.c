#include <gtk/gtk.h>
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
    char *argv[] = { full_path, NULL };
    if (!g_spawn_async(NULL, argv, NULL, G_SPAWN_DEFAULT, NULL, NULL, NULL, &error)) {
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

static GtkCssProvider *current_theme_provider = NULL;

void apply_theme(int theme_id) {
    if (current_theme_provider != NULL) {
        gtk_style_context_remove_provider_for_display(gdk_display_get_default(), GTK_STYLE_PROVIDER(current_theme_provider));
        g_object_unref(current_theme_provider);
        current_theme_provider = NULL;
    }

    if (theme_id == 1 || theme_id == 2) {
        g_object_set(gtk_settings_get_default(), "gtk-application-prefer-dark-theme", TRUE, NULL);
    } else {
        // Default theme is dark blue, so keep dark theme preferred
        g_object_set(gtk_settings_get_default(), "gtk-application-prefer-dark-theme", TRUE, NULL);
    }

    const char *theme_css = "";
    if (theme_id == 1) theme_css = "window { background-color: #11111b; } .card { background-color: #1e1e2e; color: #ffffff; border: 1px solid #45475a; box-shadow: 0 0 10px rgba(255,255,255,0.1); } label, .header-title, .subtitle, .game-title, .game-desc, .name-question, .welcome-text, .result-small-text, .result-performance-text, .attempts-text, .success-text, .big-number, .tip-text, .warning-text { color: #ffffff; font-family: sans-serif; } button, .btn-launch, .btn-settings, .btn-blue { background-color: #313244; color: #ffffff; font-weight: bold; border: 1px solid #45475a; border-radius: 8px; } button label, .btn-launch label, .btn-settings label, .btn-blue label { color: #ffffff; } button:hover, .btn-launch:hover, .btn-settings:hover, .btn-blue:hover { background-color: #45475a; }";
    else if (theme_id == 2) theme_css = "window { background-color: #0d0d0d; } .card { background-color: #000000; border: 2px solid #00ff00; box-shadow: 0 0 15px rgba(0,255,0,0.3); } label, .header-title, .subtitle, .game-title, .game-desc, .name-question, .welcome-text, .result-small-text, .result-performance-text, .attempts-text, .success-text, .big-number, .tip-text, .warning-text { color: #00ff00; font-family: monospace; } button, .btn-launch, .btn-settings, .btn-blue { background-color: #002200; color: #00ff00; border: 1px solid #00ff00; font-family: monospace; border-radius: 8px; } button label, .btn-launch label, .btn-settings label, .btn-blue label { color: #00ff00; } button:hover, .btn-launch:hover, .btn-settings:hover, .btn-blue:hover { background-color: #004400; }";
    if (theme_id != 0) {
        current_theme_provider = gtk_css_provider_new();
        gtk_css_provider_load_from_string(current_theme_provider, theme_css);
        gtk_style_context_add_provider_for_display(gdk_display_get_default(), GTK_STYLE_PROVIDER(current_theme_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    }
}
// --- END INJECTED ENGINE ---

// ============================================================
// GAME STATE
// ============================================================

typedef struct
{
    char name1[50];
    char name2[50];
    int score1;
    int score2;
    int current_round;
    int current_player;
    char board[3][3];
    int round_winners[3];
    int game_over;
} GameState;

GameState game;

// UI Widgets
GtkWidget *window;
GtkWidget *stack;
GtkWidget *entry_p1, *entry_p2;
GtkWidget *label_score_p1, *label_score_p2;
GtkWidget *buttons[3][3];
GtkWidget *result_title;
GtkWidget *result_subtitle;
GtkWidget *result_score_label;

// NEW: Dedicated error label separate from the tip
GtkWidget *lbl_start_error;

// ============================================================
// CSS STYLING
// ============================================================

const char *css_data =
    "window { background-color: #1e1e2e; }"
    "label { color: #cdd6f4; }"
    ".login-card { background-color: #ffffff; border-radius: 12px; padding: 30px; margin: 20px; box-shadow: 0px 4px 8px rgba(0,0,0,0.1); }"
    ".game-title { font-size: 16pt; font-weight: bold; color: #4a00e0; margin-bottom: 5px; }"
    ".welcome-text { font-size: 14pt; font-weight: bold; color: #2979ff; margin-bottom: 20px; }"
    ".input-label { font-size: 11pt; color: #555555; margin-bottom: 5px; }"
    ".round-header { font-size: 18pt; font-weight: bold; color: #6200ea; margin-bottom: 5px; }"
    ".score-info { font-size: 10pt; color: #666666; margin-bottom: 15px; }"
    ".styled-entry { background: #ffffff; border: 1px solid #aaa; border-radius: 4px; padding: 10px; color: #000; }"
    ".styled-entry:focus { border: 2px solid #2962ff; }"
    "#start_btn { background-color: #1a237e; background-image: none; color: white; font-weight: bold; border-radius: 5px; padding: 10px; margin-top: 15px; }"
    "#start_btn:hover { background-color: #2a3ed1; }"
    "#start_btn:active { background-color: #1123eb; box-shadow: inset 0 2px 4px rgba(0,0,0,0.2); }"
    "#start_btn:focus { border: 2px solid #534bae; }"
    ".btn-exit { background-color: #d50000; background-image: none; color: white; font-weight: bold; font-size: 16px; border-radius: 5px; padding: 10px; margin-top: 15px; }"
    ".btn-exit:hover { background-color: #b71c1c; }"
    ".btn-exit:active { background-color: #d50000; box-shadow: inset 0 2px 4px rgba(0,0,0,0.2); }"
    ".btn-exit:focus { border: 2px solid #ff5131; }"
    ".footer-tip { font-size: 9pt; color: #888888; margin-top: 15px; }"
    ".footer-credit { font-size: 8pt; color: #555555; margin-top: 5px; font-weight: bold; }"
    ".success { color: #00c853; font-weight: bold; font-size: 14pt; }"
    ".error { color: #d50000; font-weight: bold; font-size: 11pt; }"
    ".warning { color: #ffab00; font-weight: bold; font-size: 14pt; }"
    ".grid-button { font-size: 36px; font-weight: 900; min-width: 70px; min-height: 70px; background-color: #f8f9fa; border: 1px solid #dee2e6; border-radius: 8px; color: #333; }"
    ".grid-button:hover { background-color: #e9ecef; }"
    ".player-x { color: #2979ff; }"
    ".player-o { color: #d50000; }"
    
    /* NEW CSS for the specific error label under button */
    ".error-msg { color: #d50000; font-size: 10pt; font-weight: bold; margin-top: 5px; margin-bottom: 0px; }"
    ;

// ============================================================
// GAME LOGIC
// ============================================================

void init_game_state()
{
    game.current_round = 1;
    game.score1 = 0;
    game.score2 = 0;
    for (int i = 0; i < 3; i++)
        game.round_winners[i] = -1;
}

void reset_board_logic()
{
    char num = '1';
    for (int r = 0; r < 3; ++r)
    {
        for (int c = 0; c < 3; ++c)
        {
            game.board[r][c] = num++;
        }
    }
    game.current_player = (game.current_round % 2 != 0) ? 1 : 2;
    game.game_over = 0;
}

int check_winner()
{
    for (int i = 0; i < 3; i++)
    {
        if (game.board[i][0] == game.board[i][1] && game.board[i][1] == game.board[i][2]) return 1;
        if (game.board[0][i] == game.board[1][i] && game.board[1][i] == game.board[2][i]) return 1;
    }
    if (game.board[0][0] == game.board[1][1] && game.board[1][1] == game.board[2][2]) return 1;
    if (game.board[0][2] == game.board[1][1] && game.board[1][1] == game.board[2][0]) return 1;
    return 0;
}

int check_draw()
{
    for (int r = 0; r < 3; r++)
        for (int c = 0; c < 3; c++)
            if (game.board[r][c] != 'X' && game.board[r][c] != 'O') return 0;
    return 1;
}

// ============================================================
// UI UPDATES
// ============================================================

void update_ui_board()
{
    char s1[100], s2[100];
    snprintf(s1, sizeof(s1), "%s: %d", game.name1, game.score1);
    snprintf(s2, sizeof(s2), "%s: %d", game.name2, game.score2);
    gtk_label_set_text(GTK_LABEL(label_score_p1), s1);
    gtk_label_set_text(GTK_LABEL(label_score_p2), s2);

    for (int r = 0; r < 3; r++)
    {
        for (int c = 0; c < 3; c++)
        {
            char val = game.board[r][c];
            GtkWidget *btn = buttons[r][c];

            gtk_widget_remove_css_class(btn, "player-x");
            gtk_widget_remove_css_class(btn, "player-o");

            if (val == 'X') {
                gtk_button_set_label(GTK_BUTTON(btn), "X");
                gtk_widget_add_css_class(btn, "player-x");
            } else if (val == 'O') {
                gtk_button_set_label(GTK_BUTTON(btn), "O");
                gtk_widget_add_css_class(btn, "player-o");
            } else {
                char numStr[2] = {val, '\0'};
                gtk_button_set_label(GTK_BUTTON(btn), numStr);
            }
        }
    }
}

void show_result_screen()
{
    gtk_label_set_text(GTK_LABEL(result_title), "GAME OVER");

    char result_txt[100];
    if (game.score1 > game.score2) {
        snprintf(result_txt, 100, "VICTORY!\n%s won the match.", game.name1);
        gtk_widget_remove_css_class(result_subtitle, "error");
        gtk_widget_add_css_class(result_subtitle, "success");
    } else if (game.score2 > game.score1) {
        snprintf(result_txt, 100, "VICTORY!\n%s won the match.", game.name2);
        gtk_widget_remove_css_class(result_subtitle, "success");
        gtk_widget_add_css_class(result_subtitle, "error");
    } else {
        snprintf(result_txt, 100, "DRAW!\nNo clear winner.");
        gtk_widget_remove_css_class(result_subtitle, "success");
        gtk_widget_remove_css_class(result_subtitle, "error");
        gtk_widget_add_css_class(result_subtitle, "warning");
    }

    gtk_label_set_text(GTK_LABEL(result_subtitle), result_txt);

    char best_player[50];
    int best_score = load_top_score("ttt_gui", best_player);
    char score_txt[200];
    if (best_score != -1) {
        snprintf(score_txt, 200, "Final Score: %d - %d\nAll-Time Best: %s (%d wins)", game.score1, game.score2, best_player, best_score);
    } else {
        snprintf(score_txt, 200, "Final Score: %d - %d", game.score1, game.score2);
    }
    gtk_label_set_text(GTK_LABEL(result_score_label), score_txt);

    gtk_stack_set_visible_child_name(GTK_STACK(stack), "result_page");
}

void handle_round_end(int winner_id)
{
    game.game_over = 1;
    if (winner_id == 1) {
        game.score1++;
        save_score("ttt_gui", game.name1, game.score1, 0);
    } else if (winner_id == 2) {
        game.score2++;
        save_score("ttt_gui", game.name2, game.score2, 0);
    }

    game.round_winners[game.current_round - 1] = winner_id;

    if (game.current_round >= 3) {
        show_result_screen();
    } else {
        game.current_round++;
        reset_board_logic();
        update_ui_board();
    }
}

// ============================================================
// SIGNALS
// ============================================================

void on_cell_clicked(GtkWidget *widget, gpointer data)
{
    if (game.game_over) return;
    int id = GPOINTER_TO_INT(data);
    int r = id / 3;
    int c = id % 3;

    if (game.board[r][c] == 'X' || game.board[r][c] == 'O') return;

    game.board[r][c] = (game.current_player == 1) ? 'X' : 'O';

    if (check_winner()) {
        update_ui_board();
        handle_round_end(game.current_player);
    } else if (check_draw()) {
        update_ui_board();
        handle_round_end(0);
    } else {
        game.current_player = (game.current_player == 1) ? 2 : 1;
        update_ui_board();
    }
}

void on_start_clicked(GtkWidget *widget, gpointer data)
{
    const char *name = gtk_editable_get_text(GTK_EDITABLE(entry_p1));
    if (g_utf8_strlen(name, -1) == 0) {
        gtk_label_set_text(GTK_LABEL(lbl_start_error), "Please enter your name to play!");
        return;
    }
    gtk_label_set_text(GTK_LABEL(lbl_start_error), "");
    
    strncpy(game.name1, name, sizeof(game.name1) - 1);
    game.name1[sizeof(game.name1) - 1] = '\0';
    save_global_settings(game.name1, -1);
    
    snprintf(game.name2, sizeof(game.name2), "Guest");

    init_game_state();
    reset_board_logic();
    update_ui_board();
    
    gtk_stack_set_visible_child_name(GTK_STACK(stack), "game_page");
}

void on_reset_game_clicked(GtkWidget *widget, gpointer data)
{
    reset_board_logic();
    update_ui_board();
}

void on_play_again_clicked(GtkWidget *widget, gpointer data)
{
    init_game_state();
    reset_board_logic();
    update_ui_board();
    gtk_stack_set_visible_child_name(GTK_STACK(stack), "game_page");
}

static void confirm_exit_response(GObject *source_object, GAsyncResult *res, gpointer user_data) {
    GtkAlertDialog *dialog = GTK_ALERT_DIALOG(source_object);
    GError *error = NULL;
    int response = gtk_alert_dialog_choose_finish(dialog, res, &error);
    if (error) {
        g_error_free(error);
        return;
    }
    if (response == 1) {
        if (return_to_launcher()) {
            gtk_window_close(GTK_WINDOW(window));
        }
    }
}

void on_exit_clicked(GtkWidget *widget, gpointer data)
{
    GtkAlertDialog *dialog = gtk_alert_dialog_new("Are you sure you want to exit the game?");
    gtk_alert_dialog_set_detail(dialog, "Any unsaved progress will be lost.");
    const char *buttons[] = {"Cancel", "Exit", NULL};
    gtk_alert_dialog_set_buttons(dialog, buttons);
    gtk_alert_dialog_set_cancel_button(dialog, 0);
    gtk_alert_dialog_set_default_button(dialog, 0);
    
    gtk_alert_dialog_choose(dialog, GTK_WINDOW(window), NULL, confirm_exit_response, NULL);
    g_object_unref(dialog);
}

// ============================================================
// BUILD CARD HELPERS
// ============================================================

GtkWidget *create_card_box()
{
    GtkWidget *card = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_add_css_class(card, "login-card"); 
    gtk_widget_set_halign(card, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(card, GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(card, 400, -1);
    return card;
}

void add_footer(GtkWidget *box)
{
    GtkWidget *footer = gtk_label_new("Developed by SUJAY PAUL");
    gtk_widget_add_css_class(footer, "footer-credit");
    gtk_box_append(GTK_BOX(box), footer);
}

// ============================================================
// MAIN UI SETUP
// ============================================================

static void activate(GtkApplication *app, gpointer user_data)
{
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_string(provider, css_data);
    gtk_style_context_add_provider_for_display(gdk_display_get_default(), GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Epic Tic Tac Toe Battle");
    gtk_window_set_default_size(GTK_WINDOW(window), 900, 700);
    gtk_widget_add_css_class(window, "window-bg");

    stack = gtk_stack_new();
    gtk_stack_set_transition_type(GTK_STACK(stack), GTK_STACK_TRANSITION_TYPE_CROSSFADE);
    gtk_window_set_child(GTK_WINDOW(window), stack);

    // ============================================================
    // PAGE 1: START SCREEN
    // ============================================================
    GtkWidget *start_page_wrapper = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_halign(start_page_wrapper, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(start_page_wrapper, GTK_ALIGN_CENTER);

    GtkWidget *start_card = create_card_box();
    
    GtkWidget *lbl_title = gtk_label_new("🎮 TIC TAC TOE");
    gtk_widget_add_css_class(lbl_title, "game-title");
    gtk_box_append(GTK_BOX(start_card), lbl_title);

    GtkWidget *lbl_subtitle = gtk_label_new("Welcome to the Arena");
    gtk_widget_add_css_class(lbl_subtitle, "welcome-text");
    gtk_box_append(GTK_BOX(start_card), lbl_subtitle);

    GtkWidget *lbl_p1 = gtk_label_new("What is your name, Challenger?");
    gtk_widget_set_halign(lbl_p1, GTK_ALIGN_START);
    gtk_widget_add_css_class(lbl_p1, "input-label");
    gtk_box_append(GTK_BOX(start_card), lbl_p1);

    entry_p1 = gtk_entry_new();
    gtk_widget_add_css_class(entry_p1, "styled-entry");
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_p1), "Type name here...");
    gtk_box_append(GTK_BOX(start_card), entry_p1);
    
    lbl_start_error = gtk_label_new("");
    gtk_widget_add_css_class(lbl_start_error, "error-msg");
    gtk_box_append(GTK_BOX(start_card), lbl_start_error);

    GtkWidget *btn_start = gtk_button_new_with_label("START BATTLE");
    gtk_widget_set_name(btn_start, "start_btn"); 
    g_signal_connect(btn_start, "clicked", G_CALLBACK(on_start_clicked), NULL);
    gtk_box_append(GTK_BOX(start_card), btn_start);
    
    GtkWidget *btn_exit = gtk_button_new_with_label("Return to Menu");
    gtk_widget_add_css_class(btn_exit, "btn-exit"); 
    g_signal_connect(btn_exit, "clicked", G_CALLBACK(on_exit_clicked), NULL);
    gtk_box_append(GTK_BOX(start_card), btn_exit);

    gtk_box_append(GTK_BOX(start_page_wrapper), start_card);
    gtk_stack_add_named(GTK_STACK(stack), start_page_wrapper, "start_page");

    // ============================================================
    // PAGE 2: GAME BOARD
    // ============================================================
    GtkWidget *game_page_wrapper = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_halign(game_page_wrapper, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(game_page_wrapper, GTK_ALIGN_CENTER);

    GtkWidget *game_card = create_card_box();
    
    GtkWidget *lbl_round = gtk_label_new("Battle Arena");
    gtk_widget_add_css_class(lbl_round, "round-header");
    gtk_box_append(GTK_BOX(game_card), lbl_round);

    GtkWidget *box_scores = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 30);
    gtk_widget_set_halign(box_scores, GTK_ALIGN_CENTER);

    label_score_p1 = gtk_label_new("Player 1: 0");
    label_score_p2 = gtk_label_new("Player 2: 0");
    gtk_widget_add_css_class(label_score_p1, "score-info");
    gtk_widget_add_css_class(label_score_p2, "score-info");

    gtk_box_append(GTK_BOX(box_scores), label_score_p1);
    gtk_box_append(GTK_BOX(box_scores), label_score_p2);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_top(grid, 20);
    gtk_widget_set_margin_bottom(grid, 20);

    for (int i = 0; i < 9; i++) {
        int r = i / 3;
        int c = i % 3;
        buttons[r][c] = gtk_button_new_with_label("");
        gtk_widget_add_css_class(buttons[r][c], "grid-button");
        g_signal_connect(buttons[r][c], "clicked", G_CALLBACK(on_cell_clicked), GINT_TO_POINTER(i));
        gtk_grid_attach(GTK_GRID(grid), buttons[r][c], c, r, 1, 1);
    }

    GtkWidget *btn_reset = gtk_button_new_with_label("RUN IT BACK 🔄");
    gtk_widget_set_margin_top(btn_reset, 10);
    gtk_widget_set_name(btn_reset, "start_btn"); 
    g_signal_connect(btn_reset, "clicked", G_CALLBACK(on_reset_game_clicked), NULL);
    
    GtkWidget *lbl_tip_game = gtk_label_new("💡 Hint: Use your brain. It helps.");
    gtk_widget_set_margin_top(lbl_tip_game, 15);
    gtk_widget_add_css_class(lbl_tip_game, "footer-tip");

    gtk_box_append(GTK_BOX(game_card), box_scores);
    gtk_box_append(GTK_BOX(game_card), grid);
    gtk_box_append(GTK_BOX(game_card), btn_reset);
    gtk_box_append(GTK_BOX(game_card), lbl_tip_game);
    
    add_footer(game_card); 

    gtk_box_append(GTK_BOX(game_page_wrapper), game_card);
    gtk_stack_add_named(GTK_STACK(stack), game_page_wrapper, "game_page");

    // ============================================================
    // PAGE 3: RESULT SCREEN
    // ============================================================
    GtkWidget *result_page_wrapper = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_halign(result_page_wrapper, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(result_page_wrapper, GTK_ALIGN_CENTER);

    GtkWidget *result_card = create_card_box();

    result_title = gtk_label_new("GAME OVER");
    gtk_widget_add_css_class(result_title, "game-title");

    result_subtitle = gtk_label_new("");
    gtk_label_set_justify(GTK_LABEL(result_subtitle), GTK_JUSTIFY_CENTER);
    gtk_widget_set_margin_bottom(result_subtitle, 10);
    gtk_widget_add_css_class(result_subtitle, "welcome-text");

    result_score_label = gtk_label_new("Final Score: 0 - 0");
    gtk_widget_add_css_class(result_score_label, "score-info");

    GtkWidget *box_actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_set_halign(box_actions, GTK_ALIGN_CENTER);

    GtkWidget *btn_rematch = gtk_button_new_with_label("Rematch?");
    gtk_widget_set_name(btn_rematch, "start_btn"); 
    g_signal_connect(btn_rematch, "clicked", G_CALLBACK(on_play_again_clicked), NULL);

    GtkWidget *btn_end = gtk_button_new_with_label("Return to Menu");
    gtk_widget_add_css_class(btn_end, "btn-exit"); 
    g_signal_connect(btn_end, "clicked", G_CALLBACK(on_exit_clicked), NULL);

    gtk_box_append(GTK_BOX(box_actions), btn_rematch);
    gtk_box_append(GTK_BOX(box_actions), btn_end);

    gtk_box_append(GTK_BOX(result_card), result_title);
    gtk_box_append(GTK_BOX(result_card), result_subtitle);
    gtk_box_append(GTK_BOX(result_card), result_score_label);
    gtk_box_append(GTK_BOX(result_card), box_actions);
    add_footer(result_card);

    gtk_box_append(GTK_BOX(result_page_wrapper), result_card);
    gtk_stack_add_named(GTK_STACK(stack), result_page_wrapper, "result_page");
    
    int theme_id;
    char player_name[50];
    load_global_settings(player_name, &theme_id);
    apply_theme(theme_id);
    
    /* Set default player name if available */
    if (strlen(player_name) > 0) {
        gtk_editable_set_text(GTK_EDITABLE(entry_p1), player_name);
    }
    
    gtk_stack_set_visible_child_name(GTK_STACK(stack), "start_page");

    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char **argv)
{
    GtkApplication *app = gtk_application_new("org.sujay.tictactoe", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}