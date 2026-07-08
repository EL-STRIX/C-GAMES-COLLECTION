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
#include "../common/persistence.h"

#ifdef _WIN32
#include <windows.h>
#endif

/* --- Game Constants --- */
#define CHOICE_ROCK 1
#define CHOICE_PAPER 2
#define CHOICE_SCISSORS 3
#define TOTAL_ROUNDS 3

/* --- Data Structure --- */
// AppData encapsulates the application's runtime state and widget references,
// eliminating globals and ensuring memory safety across callback boundaries.
typedef struct {
    GtkWidget *window; 
    int current_round;         
    int player_score;          
    int computer_score;        
    char player_name[50];      

    GtkWidget *stack;          

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

static void on_start_clicked(GtkButton *btn, AppData *data) {
    const char *name = gtk_editable_get_text(GTK_EDITABLE(data->name_entry));
    char *trimmed = g_strstrip(g_strdup(name));
    if (g_utf8_strlen(trimmed, -1) == 0) {
        gtk_label_set_text(GTK_LABEL(data->name_error_label), "Please enter your name to play!");
        g_free(trimmed);
        return;
    }
    gtk_label_set_text(GTK_LABEL(data->name_error_label), "");
    strncpy(data->player_name, trimmed, sizeof(data->player_name) - 1);
    g_free(trimmed);
    data->player_name[sizeof(data->player_name) - 1] = '\0';
    save_global_settings(data->player_name, -1);
    
    start_new_game(data);
    gtk_stack_set_visible_child_name(GTK_STACK(data->stack), "game_screen");
}

/* Simple wrappers connecting each choice button to process_round() */
void on_rock_clicked(GtkButton *btn, gpointer user_data) { process_round((AppData*)user_data, CHOICE_ROCK); }
void on_paper_clicked(GtkButton *btn, gpointer user_data) { process_round((AppData*)user_data, CHOICE_PAPER); }
void on_scissors_clicked(GtkButton *btn, gpointer user_data) { process_round((AppData*)user_data, CHOICE_SCISSORS); }
void on_next_round_clicked(GtkButton *btn, gpointer user_data) { start_next_round_ui((AppData*)user_data); }
void on_play_again_clicked(GtkButton *btn, gpointer user_data) { start_new_game((AppData*)user_data); }

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
            AppData *data = (AppData *)user_data;
            GtkWindow *window = GTK_WINDOW(data->window);
            GtkApplication *app = gtk_window_get_application(window);
            g_application_quit(G_APPLICATION(app));
        }
    }
}

/* Back Button Callback - returns to menu with confirmation if playing */
void on_header_back_clicked(GtkButton *btn, gpointer user_data) {
    AppData *data = (AppData *)user_data;
    const char *visible_child = gtk_stack_get_visible_child_name(GTK_STACK(data->stack));
    if (g_strcmp0(visible_child, "game_screen") == 0) {
        GtkAlertDialog *dialog = gtk_alert_dialog_new("Are you sure you want to return to the main menu?");
        gtk_alert_dialog_set_detail(dialog, "Any unsaved progress will be lost.");
        const char *buttons[] = {"Cancel", "Return to Menu", NULL};
        gtk_alert_dialog_set_buttons(dialog, buttons);
        gtk_alert_dialog_set_cancel_button(dialog, 0);
        gtk_alert_dialog_set_default_button(dialog, 0);
        gtk_alert_dialog_choose(dialog, GTK_WINDOW(data->window), NULL, confirm_exit_response, data);
        g_object_unref(dialog);
    } else {
        if (return_to_launcher()) {
            GtkApplication *app = gtk_window_get_application(GTK_WINDOW(data->window));
            g_application_quit(G_APPLICATION(app));
        }
    }
}

/* --- CSS Styling --- */
/* Loads application CSS into the GTK style context */
void load_css(void) {
    load_css_from_file("theme_white_blue.css");
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

/* 1. Login Screen */
GtkWidget* create_login_screen(AppData *data) {
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    gtk_widget_set_valign(vbox, GTK_ALIGN_CENTER);
    gtk_widget_set_halign(vbox, GTK_ALIGN_CENTER);
    gtk_widget_set_vexpand(vbox, TRUE);
    gtk_widget_set_hexpand(vbox, TRUE);

    GtkWidget *card = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_add_css_class(card, "login-card");
    gtk_widget_set_size_request(card, 350, -1);
    gtk_box_append(GTK_BOX(vbox), card);

    GtkWidget *title_lbl = gtk_label_new("🎮 ROCK PAPER SCISSORS");
    gtk_widget_add_css_class(title_lbl, "game-title");
    gtk_box_append(GTK_BOX(card), title_lbl);

    GtkWidget *welcome_lbl = gtk_label_new("Welcome to the Arena");
    gtk_widget_add_css_class(welcome_lbl, "welcome-text");
    gtk_box_append(GTK_BOX(card), welcome_lbl);

    GtkWidget *input_lbl = gtk_label_new("What is your name, Challenger?");
    gtk_widget_add_css_class(input_lbl, "input-label");
    gtk_widget_set_halign(input_lbl, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(card), input_lbl);

    data->name_entry = gtk_entry_new();
    gtk_widget_add_css_class(data->name_entry, "styled-entry");
    gtk_entry_set_placeholder_text(GTK_ENTRY(data->name_entry), "Type name here...");
    gtk_box_append(GTK_BOX(card), data->name_entry);

    data->name_error_label = gtk_label_new("");
    gtk_widget_add_css_class(data->name_error_label, "error");
    gtk_box_append(GTK_BOX(card), data->name_error_label);

    GtkWidget *start_btn = gtk_button_new_with_label("START BATTLE");
    gtk_widget_set_name(start_btn, "start_btn");
    g_signal_connect(start_btn, "clicked", G_CALLBACK(on_start_clicked), data);
    gtk_box_append(GTK_BOX(card), start_btn);

    return vbox;
}
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
    gtk_window_set_default_size(GTK_WINDOW(window), 900, 700);
    gtk_window_maximize(GTK_WINDOW(window));
    
    /* Store the window in AppData so the Exit button can use it */
    data->window = window;

    GtkWidget *header = gtk_header_bar_new();
    gtk_header_bar_set_show_title_buttons(GTK_HEADER_BAR(header), TRUE);
    gtk_window_set_titlebar(GTK_WINDOW(window), header);

    GtkWidget *title_lbl = gtk_label_new("Epic Rock Paper Scissors Battle");
    gtk_widget_add_css_class(title_lbl, "header-title");
    gtk_header_bar_set_title_widget(GTK_HEADER_BAR(header), title_lbl);

    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_add_css_class(main_box, "window-bg");
    // Global Overlay for the "Return to Main Menu" button
    GtkWidget *overlay = gtk_overlay_new();
    gtk_overlay_set_child(GTK_OVERLAY(overlay), main_box);

    GtkWidget *global_btn_back = gtk_button_new_with_label("🔙 Return to Main Menu");
    gtk_widget_set_halign(global_btn_back, GTK_ALIGN_START);
    gtk_widget_set_valign(global_btn_back, GTK_ALIGN_START);
    gtk_widget_set_margin_top(global_btn_back, 15);
    gtk_widget_set_margin_end(global_btn_back, 15);
    gtk_widget_add_css_class(global_btn_back, "btn-exit");
    g_signal_connect(global_btn_back, "clicked", G_CALLBACK(on_header_back_clicked), data);

    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), global_btn_back);

    gtk_window_set_child(GTK_WINDOW(window), overlay);

    data->stack = gtk_stack_new();
    gtk_stack_set_transition_type(GTK_STACK(data->stack), GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
    
    gtk_widget_set_vexpand(data->stack, TRUE);
    gtk_widget_set_hexpand(data->stack, TRUE);

    gtk_box_append(GTK_BOX(main_box), data->stack);

    /* add the three screens to the stack */
    gtk_stack_add_named(GTK_STACK(data->stack), create_login_screen(data), "login_screen");
    gtk_stack_add_named(GTK_STACK(data->stack), create_game_screen(data), "game_screen");
    gtk_stack_add_named(GTK_STACK(data->stack), create_result_screen(data), "result_screen");

    load_css(); /* apply app CSS */
    
    int theme_id;
    load_global_settings(data->player_name, sizeof(data->player_name), &theme_id);
    // apply_theme(theme_id);
    
    /* Set default player name if available */
    if (strlen(data->player_name) > 0) {
        gtk_editable_set_text(GTK_EDITABLE(data->name_entry), data->player_name);
    }

    gtk_widget_set_visible(data->next_round_btn, FALSE);

    if (strlen(data->player_name) > 0 && strcmp(data->player_name, "Player 1") != 0) {
        start_new_game(data);
        gtk_stack_set_visible_child_name(GTK_STACK(data->stack), "game_screen");
    } else {
        gtk_stack_set_visible_child_name(GTK_STACK(data->stack), "login_screen");
    }

    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char **argv) {
    GtkApplication *app = gtk_application_new("org.sujay.rps", G_APPLICATION_NON_UNIQUE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}