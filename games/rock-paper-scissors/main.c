/*
 * ----------------------------------------------------------------------------
 * Project: Rock Paper Scissors (GTK4)
 * Developer: Sujay Paul
 * Date: 10-12-2025
 * Email: sujaypaul892@gmail.com
 * ----------------------------------------------------------------------------
 * NOTE: This file implements a Rock-Paper-Scissors GUI using GTK4.
 * Short comments were added throughout for readability — code logic remains unchanged.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <gtk/gtk.h>
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
        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
            "Failed to return to launcher: %s\nPath: %s", error->message, full_path);
        g_signal_connect(dialog, "response", G_CALLBACK(gtk_window_destroy), NULL);
        gtk_window_present(GTK_WINDOW(dialog));
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

/* --- Game Constants --- */
#define CHOICE_ROCK 1
#define CHOICE_PAPER 2
#define CHOICE_SCISSORS 3
#define TOTAL_ROUNDS 3

/* --- Data Structure --- */
typedef struct {
    GtkWidget *window; 
    int current_round;          /* current round index (1-based) */
    int player_score;          /* player cumulative score */
    int computer_score;        /* computer cumulative score */
    char player_name[50];      /* stored player name from login */

    GtkWidget *stack;          /* main UI stack with screens */

    /* Screen 1 (Login) */
    GtkWidget *name_entry;     /* entry widget for player's name */
    GtkWidget *name_error_label; /* label to show validation error */

    /* Screen 2 (Game) */
    GtkWidget *round_label;    /* label showing current round */
    GtkWidget *score_label;    /* label showing live scores */
    GtkWidget *feedback_label; /* label showing last choices */
    GtkWidget *result_label;   /* label showing round result (win/lose/draw) */
    GtkWidget *choices_box;    /* container for rock/paper/scissor buttons */
    GtkWidget *next_round_btn; /* button to proceed to next round */

    /* Screen 3 (Result) */
    GtkWidget *final_outcome_label; /* large label for final winner */
    GtkWidget *final_score_label;   /* final score display */
    GtkWidget *play_again_btn;      /* restart game button */
} AppData;

/* --- Forward Declarations --- */
static void start_new_game(AppData *data);
static void start_next_round_ui(AppData *data);
static void process_round(AppData *data, int user_choice);

/* --- Helpers --- */
/* Update the score label text using current names and scores */
void update_score_display(AppData *data) {
    char *text = g_strdup_printf("%s: %d  |  Computer: %d",
                                  data->player_name[0] ? data->player_name : "Player",
                                  data->player_score, data->computer_score);
    gtk_label_set_text(GTK_LABEL(data->score_label), text);
    g_free(text);
}

/* Update the round header label depending on current round */
void update_round_display(AppData *data) {
    char *text;
    if (data->current_round <= TOTAL_ROUNDS) {
        text = g_strdup_printf("Round %d: Fight!", data->current_round);
    } else {
        /* when rounds are over show a calculating message */
        text = g_strdup("Calculating Results...");
    }
    gtk_label_set_text(GTK_LABEL(data->round_label), text);
    g_free(text);
}

/* Timer callback to compute and show final results -- runs in main loop */
gboolean on_show_final_results(gpointer user_data) {
    AppData *data = (AppData *)user_data;
    char *outcome_text;
    char *score_text;

    /* clear any previous CSS classes on the final label */
    gtk_widget_remove_css_class(data->final_outcome_label, "success");
    gtk_widget_remove_css_class(data->final_outcome_label, "error");
    gtk_widget_remove_css_class(data->final_outcome_label, "warning");

    /* Determine winner and set appropriate text and CSS class */
    if (data->player_score > data->computer_score) {
        outcome_text = g_strdup_printf("CHAMPION!\n%s wins!", data->player_name);
        gtk_widget_add_css_class(data->final_outcome_label, "success");
    } else if (data->computer_score > data->player_score) {
        outcome_text = g_strdup("DEFEAT!\nThe Computer won.");
        gtk_widget_add_css_class(data->final_outcome_label, "error");
    } else {
        outcome_text = g_strdup("DRAW GAME!");
        gtk_widget_add_css_class(data->final_outcome_label, "warning");
    }

    if (data->player_score > data->computer_score) {
        save_score("rps", data->player_name, data->player_score, 0);
    }
    char best_player[50];
    int best_score = load_top_score("rps", best_player);

    if (best_score != -1) {
        score_text = g_strdup_printf("Final Score: %d - %d\nAll-Time Best: %s (%d wins)", data->player_score, data->computer_score, best_player, best_score);
    } else {
        score_text = g_strdup_printf("Final Score: %d - %d", data->player_score, data->computer_score);
    }

    /* update UI labels and switch to result screen */
    gtk_label_set_text(GTK_LABEL(data->final_outcome_label), outcome_text);
    gtk_label_set_text(GTK_LABEL(data->final_score_label), score_text);

    g_free(outcome_text);
    g_free(score_text);

    gtk_stack_set_visible_child_name(GTK_STACK(data->stack), "result_screen");
    return G_SOURCE_REMOVE; /* stop the timeout source after running once */
}

/* --- Game Logic --- */
/* Initialize and start a fresh game */
void start_new_game(AppData *data) {
    data->current_round = 1;
    data->player_score = 0;
    data->computer_score = 0;

    /* reset feedback/result labels */
    gtk_label_set_text(GTK_LABEL(data->feedback_label), "Make your move...");
    gtk_label_set_text(GTK_LABEL(data->result_label), "");
    
    /* remove any previous result styling */
    gtk_widget_remove_css_class(data->result_label, "success");
    gtk_widget_remove_css_class(data->result_label, "error");
    gtk_widget_remove_css_class(data->result_label, "warning");

    update_round_display(data);
    update_score_display(data);

    /* make choices visible and hide next button until a round is played */
    gtk_widget_set_visible(data->choices_box, TRUE);
    gtk_widget_set_visible(data->next_round_btn, FALSE);

    /* show the game screen in the stack */
    gtk_stack_set_visible_child_name(GTK_STACK(data->stack), "game_screen");
}

/* Prepare UI for the next round (clears previous messages) */
void start_next_round_ui(AppData *data) {
    gtk_label_set_text(GTK_LABEL(data->feedback_label), "Make your move...");
    gtk_label_set_text(GTK_LABEL(data->result_label), "");
    
    gtk_widget_remove_css_class(data->result_label, "success");
    gtk_widget_remove_css_class(data->result_label, "error");
    gtk_widget_remove_css_class(data->result_label, "warning");

    update_round_display(data);
    update_score_display(data);

    gtk_widget_set_visible(data->choices_box, TRUE);
    gtk_widget_set_visible(data->next_round_btn, FALSE);
}

/* Process a single round: generate computer choice, decide winner, update UI */
void process_round(AppData *data, int user_choice) {
    int computer_choice = (rand() % 3) + 1; /* random int in 1..3 */
    const char *user_str = (user_choice == 1) ? "ROCK" : (user_choice == 2) ? "PAPER" : "SCISSORS";
    const char *comp_str = (computer_choice == 1) ? "ROCK" : (computer_choice == 2) ? "PAPER" : "SCISSORS";
    int result = 0; /* 0 draw, 1 player win, 2 computer win */

    /* compare choices to determine result and update scores */
    if (user_choice == computer_choice) result = 0;
    else if ((user_choice == 1 && computer_choice == 3) ||
             (user_choice == 2 && computer_choice == 1) ||
             (user_choice == 3 && computer_choice == 2)) {
        result = 1;
        data->player_score++;
    } else {
        result = 2;
        data->computer_score++;
    }

    /* show which choices were made */
    char *fb_text = g_strdup_printf("You: %s  vs  PC: %s", user_str, comp_str);
    gtk_label_set_text(GTK_LABEL(data->feedback_label), fb_text);
    g_free(fb_text);

    /* clear old styling from the result label */
    gtk_widget_remove_css_class(data->result_label, "success");
    gtk_widget_remove_css_class(data->result_label, "error");
    gtk_widget_remove_css_class(data->result_label, "warning");

    /* set round result text and styling */
    if (result == 0) {
        gtk_label_set_text(GTK_LABEL(data->result_label), "It's a Draw.");
        gtk_widget_add_css_class(data->result_label, "warning");
    } else if (result == 1) {
        gtk_label_set_text(GTK_LABEL(data->result_label), "You Won!");
        gtk_widget_add_css_class(data->result_label, "success");
    } else {
        gtk_label_set_text(GTK_LABEL(data->result_label), "Computer Won.");
        gtk_widget_add_css_class(data->result_label, "error");
    }

    update_score_display(data);
    gtk_widget_set_visible(data->choices_box, FALSE); /* hide choice buttons after play */

    if (data->current_round < TOTAL_ROUNDS) {
        data->current_round++;
        gtk_button_set_label(GTK_BUTTON(data->next_round_btn), "Next Round ->");
        gtk_widget_set_visible(data->next_round_btn, TRUE); /* show next button */
    } else {
        /* increment once more so update_round_display() shows calculation state in callback */
        data->current_round++;
        /* schedule final result display after 1 second */
        g_timeout_add_seconds(1, on_show_final_results, data);
    }
}

/* Removed on_start_clicked as we bypass login */

/* Simple wrappers connecting each choice button to process_round() */
void on_rock_clicked(GtkButton *btn, gpointer user_data) { process_round((AppData*)user_data, CHOICE_ROCK); }
void on_paper_clicked(GtkButton *btn, gpointer user_data) { process_round((AppData*)user_data, CHOICE_PAPER); }
void on_scissors_clicked(GtkButton *btn, gpointer user_data) { process_round((AppData*)user_data, CHOICE_SCISSORS); }
void on_next_round_clicked(GtkButton *btn, gpointer user_data) { start_next_round_ui((AppData*)user_data); }
void on_play_again_clicked(GtkButton *btn, gpointer user_data) { start_new_game((AppData*)user_data); }

/* Exit Callback - quits the application cleanly */
void on_exit_clicked(GtkButton *btn, gpointer user_data) {
    if (return_to_launcher()) {
        AppData *data = (AppData *)user_data;
        GtkWindow *window = GTK_WINDOW(data->window);
        GtkApplication *app = gtk_window_get_application(window);
        g_application_quit(G_APPLICATION(app)); /* Added cast here */
    }
}

/* --- CSS Styling --- */
/* Loads application CSS into the GTK style context */
void load_css(void) {
    GtkCssProvider *provider = gtk_css_provider_new();
    GdkDisplay *display = gdk_display_get_default();

    const char *css =
        /* Main background - Grey */
        ".window-bg { background-color: #cfcfcf; }"
        
        /* The White Card */
        ".login-card { background-color: #ffffff; border-radius: 12px; padding: 30px; margin: 20px; box-shadow: 0px 4px 8px rgba(0,0,0,0.1); }"
        
        /* Typography */
        ".game-title { font-size: 16pt; font-weight: bold; color: #4a00e0; margin-bottom: 5px; }"
        ".welcome-text { font-size: 14pt; font-weight: bold; color: #2979ff; margin-bottom: 20px; }"
        ".input-label { font-size: 11pt; color: #555555; margin-bottom: 5px; }"
        ".round-header { font-size: 18pt; font-weight: bold; color: #6200ea; margin-bottom: 5px; }"
        ".score-info { font-size: 10pt; color: #666666; margin-bottom: 15px; }"

        /* Entry Field */
        ".styled-entry { background: #ffffff; border: 1px solid #aaa; border-radius: 4px; padding: 10px; color: #000; }"
        ".styled-entry:focus { border: 2px solid #2962ff; }"
        
        /* Added :focus and :active, and background-image: none */
        "#start_btn { background-color: #1a237e; background-image: none; color: white; font-weight: bold; border-radius: 5px; padding: 10px; margin-top: 15px; }"
        "#start_btn:hover { background-color: #2a3ed1ff; }"
        "#start_btn:active { background-color: #1123ebff; box-shadow: inset 0 2px 4px rgba(0,0,0,0.2); }"
        "#start_btn:focus { border: 2px solid #534bae; }"

        /* Added :focus and :active, and background-image: none */
        ".btn-exit { background-color: #d50000; background-image: none; color: white; font-weight: bold; font-size: 16px; border-radius: 5px; padding: 10px; margin-top: 15px; }"
        ".btn-exit:hover { background-color: #b71c1c; }"
        ".btn-exit:active { background-color: #d50000; box-shadow: inset 0 2px 4px rgba(0,0,0,0.2); }"
        ".btn-exit:focus { border: 2px solid #ff5131; }"

        /* Choice Buttons (Rock/Paper/Scissors) */
        ".choice-btn { background-color: #f8f9fa; border: 1px solid #dee2e6; border-radius: 8px; padding: 10px; box-shadow: 0 2px 2px rgba(0,0,0,0.05); }"
        ".choice-btn:hover { background-color: #e9ecef; border-color: #adb5bd; }"
        ".choice-emoji { font-size: 36px; }"
        ".choice-label { font-weight: bold; color: #333; font-size: 16px; margin-top: 5px; }"
        
        /* Footer Text */
        ".footer-tip { font-size: 9pt; color: #888888; margin-top: 15px; }"
        ".footer-credit { font-size: 8pt; color: #555555; margin-top: 5px; font-weight: bold; }"
        
        /* Game Screen Elements */
        ".success { color: #00c853; font-weight: bold; font-size: 14pt; }"
        ".error { color: #d50000; font-weight: bold; font-size: 11pt; }"
        ".warning { color: #ffab00; font-weight: bold; font-size: 14pt; }";

    gtk_css_provider_load_from_string(provider, css);

    if (display)
        gtk_style_context_add_provider_for_display(display, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    g_object_unref(provider);
}

/* --- UI Construction --- */

/* Helper to build a choice button with emoji + label */
GtkWidget* create_choice_button(const char *emoji, const char *label_text, GCallback callback, AppData *data) {
    GtkWidget *btn = gtk_button_new();
    gtk_widget_add_css_class(btn, "choice-btn");
    
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    gtk_widget_set_halign(box, GTK_ALIGN_CENTER);

    GtkWidget *lbl_emoji = gtk_label_new(emoji);
    gtk_widget_add_css_class(lbl_emoji, "choice-emoji");
    
    GtkWidget *lbl_text = gtk_label_new(label_text);
    gtk_widget_add_css_class(lbl_text, "choice-label");

    gtk_box_append(GTK_BOX(box), lbl_emoji);
    gtk_box_append(GTK_BOX(box), lbl_text);

    gtk_button_set_child(GTK_BUTTON(btn), box);
    g_signal_connect(btn, "clicked", callback, data);

    return btn;
}

/* Removed create_name_screen */
/* 2. Game Screen */
/* Main gameplay screen: shows rounds, scores, and rock-paper-scissors buttons. */
GtkWidget* create_game_screen(AppData *data) {
    GtkWidget *center_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_valign(center_box, GTK_ALIGN_CENTER);
    gtk_widget_set_halign(center_box, GTK_ALIGN_CENTER);
    gtk_widget_set_vexpand(center_box, TRUE);
    gtk_widget_set_hexpand(center_box, TRUE);

    GtkWidget *card = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_add_css_class(card, "login-card"); 
    gtk_widget_set_size_request(card, 380, -1);
    gtk_box_append(GTK_BOX(center_box), card);

    GtkWidget *title_lbl = gtk_label_new("🎮 ROCK PAPER SCISSORS");
    gtk_widget_add_css_class(title_lbl, "game-title");
    gtk_box_append(GTK_BOX(card), title_lbl);

    data->round_label = gtk_label_new("Round 1: Fight!");
    gtk_widget_add_css_class(data->round_label, "round-header");
    gtk_box_append(GTK_BOX(card), data->round_label);

    data->score_label = gtk_label_new("Player: 0 | Computer: 0");
    gtk_widget_add_css_class(data->score_label, "score-info");
    gtk_box_append(GTK_BOX(card), data->score_label);

    data->feedback_label = gtk_label_new("Make your move...");
    gtk_widget_set_margin_bottom(data->feedback_label, 15);
    gtk_box_append(GTK_BOX(card), data->feedback_label);

    data->choices_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 15);
    gtk_widget_set_halign(data->choices_box, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_bottom(data->choices_box, 10);

    /* Custom Buttons - USING EMOJIS */
    GtkWidget *btn_rock = create_choice_button("✊", "Rock", G_CALLBACK(on_rock_clicked), data);
    GtkWidget *btn_paper = create_choice_button("✋", "Paper", G_CALLBACK(on_paper_clicked), data);
    GtkWidget *btn_scissors = create_choice_button("✌️", "Scissors", G_CALLBACK(on_scissors_clicked), data);
    
    gtk_widget_set_size_request(btn_rock, 80, 80);
    gtk_widget_set_size_request(btn_paper, 80, 80);
    gtk_widget_set_size_request(btn_scissors, 80, 80);

    gtk_box_append(GTK_BOX(data->choices_box), btn_rock);
    gtk_box_append(GTK_BOX(data->choices_box), btn_paper);
    gtk_box_append(GTK_BOX(data->choices_box), btn_scissors);

    gtk_box_append(GTK_BOX(card), data->choices_box);

    data->result_label = gtk_label_new("");
    gtk_box_append(GTK_BOX(card), data->result_label);

    data->next_round_btn = gtk_button_new_with_label("Next");
    gtk_widget_set_name(data->next_round_btn, "start_btn");
    g_signal_connect(data->next_round_btn, "clicked", G_CALLBACK(on_next_round_clicked), data);
    gtk_box_append(GTK_BOX(card), data->next_round_btn);

    GtkWidget *credit_lbl = gtk_label_new("Developed by SUJAY PAUL");
    gtk_widget_add_css_class(credit_lbl, "footer-credit");
    gtk_box_append(GTK_BOX(card), credit_lbl);

    return center_box;
}

/* 3. Result Screen (MODIFIED) */
/* Final summary screen: displays winner, final score, and options to restart or exit. */
GtkWidget* create_result_screen(AppData *data) {
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    gtk_widget_set_valign(vbox, GTK_ALIGN_CENTER);
    gtk_widget_set_halign(vbox, GTK_ALIGN_CENTER);
    gtk_widget_set_vexpand(vbox, TRUE);
    gtk_widget_set_hexpand(vbox, TRUE);

    GtkWidget *card = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    gtk_widget_add_css_class(card, "login-card");
    gtk_widget_set_size_request(card, 350, -1);
    gtk_box_append(GTK_BOX(vbox), card);

    GtkWidget *title_lbl = gtk_label_new("🎮 ROCK PAPER SCISSORS");
    gtk_widget_add_css_class(title_lbl, "game-title");
    gtk_box_append(GTK_BOX(card), title_lbl);

    data->final_outcome_label = gtk_label_new("");
    gtk_widget_set_halign(data->final_outcome_label, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(card), data->final_outcome_label);

    data->final_score_label = gtk_label_new("");
    gtk_widget_set_halign(data->final_score_label, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(card), data->final_score_label);

    /* --- SIDE BY SIDE BUTTONS CONTAINER --- */
    GtkWidget *button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_set_halign(button_box, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_top(button_box, 15);

    /* 1. Play Again Button */
    data->play_again_btn = gtk_button_new_with_label("Rematch?");
    gtk_widget_set_name(data->play_again_btn, "start_btn");
    gtk_widget_set_hexpand(data->play_again_btn, TRUE);
    g_signal_connect(data->play_again_btn, "clicked", G_CALLBACK(on_play_again_clicked), data);
    gtk_box_append(GTK_BOX(button_box), data->play_again_btn);

    /* 2. Exit Button */
    GtkWidget *exit_btn = gtk_button_new_with_label("Return to Menu");
    gtk_widget_add_css_class(exit_btn, "btn-exit");
    gtk_widget_set_hexpand(exit_btn, TRUE);
    g_signal_connect(exit_btn, "clicked", G_CALLBACK(on_exit_clicked), data);
    gtk_box_append(GTK_BOX(button_box), exit_btn);

    /* Add the button box to the card */
    gtk_box_append(GTK_BOX(card), button_box);
    /* -------------------------------------- */

    GtkWidget *credit_lbl = gtk_label_new("Developed by SUJAY PAUL");
    gtk_widget_add_css_class(credit_lbl, "footer-credit");
    gtk_box_append(GTK_BOX(card), credit_lbl);

    return vbox;
}

void activate(GtkApplication *app, gpointer user_data) {
    (void)user_data;
    AppData *data = g_slice_new0(AppData);
    srand((unsigned int)time(NULL)); /* seed RNG for computer choice */

    GtkWidget *window = gtk_application_window_new(app);
    gtk_widget_set_size_request(window, 800, 600);
    
    /* Store the window in AppData so the Exit button can use it */
    data->window = window;

    GtkWidget *header = gtk_header_bar_new();
    gtk_window_set_titlebar(GTK_WINDOW(window), header);
    gtk_window_set_title(GTK_WINDOW(window), "Epic Rock Paper Scissors Battle");

    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_add_css_class(main_box, "window-bg");
    gtk_window_set_child(GTK_WINDOW(window), main_box);

    data->stack = gtk_stack_new();
    gtk_stack_set_transition_type(GTK_STACK(data->stack), GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
    
    gtk_widget_set_vexpand(data->stack, TRUE);
    gtk_widget_set_hexpand(data->stack, TRUE);

    gtk_box_append(GTK_BOX(main_box), data->stack);

    /* add the three screens to the stack */
    gtk_stack_add_named(GTK_STACK(data->stack), create_game_screen(data), "game_screen");
    gtk_stack_add_named(GTK_STACK(data->stack), create_result_screen(data), "result_screen");

    load_css(); /* apply app CSS */
    
    int theme_id;
    load_global_settings(data->player_name, &theme_id);
    apply_theme(theme_id);

    gtk_window_present(GTK_WINDOW(window));

    gtk_widget_set_visible(data->next_round_btn, FALSE);
    start_new_game(data);
}

int main(int argc, char **argv) {
    GtkApplication *app = gtk_application_new("org.sujay.rps", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}