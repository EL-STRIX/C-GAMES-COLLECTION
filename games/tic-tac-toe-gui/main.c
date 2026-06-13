#include <gtk/gtk.h>
#include <string.h>
#include "../../common/persistence.h"

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
    ".window-bg { background-color: #cfcfcf; }"
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
    char s1[50], s2[50];
    snprintf(s1, 50, "%s: %d", game.name1, game.score1);
    snprintf(s2, 50, "%s: %d", game.name2, game.score2);
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

// Helper: Focus on the second entry field
void focus_entry_2(GtkWidget *widget, gpointer data) {
    gtk_widget_grab_focus(entry_p2);
}

void on_start_clicked(GtkWidget *widget, gpointer data)
{
    const char *n1 = gtk_editable_get_text(GTK_EDITABLE(entry_p1));
    const char *n2 = gtk_editable_get_text(GTK_EDITABLE(entry_p2));

    // UPDATED VALIDATION LOGIC
    if (strlen(n1) == 0) {
        gtk_label_set_text(GTK_LABEL(lbl_start_error), "IDENTIFY YOURSELF! Name is required. 🛡️");
        gtk_widget_grab_focus(entry_p1); // Focus back to P1
        return;
    }

    if (strlen(n2) == 0) {
        gtk_label_set_text(GTK_LABEL(lbl_start_error), "CHALLENGER MISSING! Don't be shy. ⚔️");
        gtk_widget_grab_focus(entry_p2); // Focus back to P2
        return;
    }

    // Clear error if success
    gtk_label_set_text(GTK_LABEL(lbl_start_error), "");

    strncpy(game.name1, n1, 49);
    strncpy(game.name2, n2, 49);

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

void on_exit_clicked(GtkWidget *widget, gpointer data)
{
    gtk_window_close(GTK_WINDOW(window));
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
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
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

    GtkWidget *lbl_icon = gtk_label_new("🎮 TIC TAC TOE");
    gtk_widget_add_css_class(lbl_icon, "game-title"); 

    GtkWidget *lbl_welcome = gtk_label_new("Welcome!");
    gtk_widget_add_css_class(lbl_welcome, "welcome-text"); 

    GtkWidget *lbl_prompt = gtk_label_new("Select your fighter:");
    gtk_widget_add_css_class(lbl_prompt, "input-label");

    entry_p1 = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_p1), "User_Alpha...");
    gtk_widget_add_css_class(entry_p1, "styled-entry");
    
    // BETTER UX: Pressing Enter on P1 jumps to P2
    g_signal_connect(entry_p1, "activate", G_CALLBACK(focus_entry_2), NULL);

    entry_p2 = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_p2), "User_Beta...");
    gtk_widget_add_css_class(entry_p2, "styled-entry");
    
    // Pressing Enter on P2 starts the game
    g_signal_connect(entry_p2, "activate", G_CALLBACK(on_start_clicked), NULL);

    GtkWidget *btn_start = gtk_button_new_with_label("LOCK IN ⚔️");
    gtk_widget_set_name(btn_start, "start_btn"); 
    g_signal_connect(btn_start, "clicked", G_CALLBACK(on_start_clicked), NULL);

    // --- ERROR MESSAGE (Initially Empty) ---
    // This is placed RIGHT UNDER the button
    lbl_start_error = gtk_label_new("");
    gtk_widget_add_css_class(lbl_start_error, "error-msg");

    // Tip Label (Bottom)
    GtkWidget *lbl_tip_start = gtk_label_new("💡 Hint: Use your brain. It helps.");
    gtk_widget_add_css_class(lbl_tip_start, "footer-tip");

    gtk_box_append(GTK_BOX(start_card), lbl_icon);
    gtk_box_append(GTK_BOX(start_card), lbl_welcome);
    gtk_box_append(GTK_BOX(start_card), lbl_prompt);
    
    gtk_box_append(GTK_BOX(start_card), entry_p1);
    gtk_box_append(GTK_BOX(start_card), entry_p2);
    
    gtk_box_append(GTK_BOX(start_card), btn_start);
    
    // Append Error Label Here
    gtk_box_append(GTK_BOX(start_card), lbl_start_error);
    
    // Append Tip Label Here
    gtk_box_append(GTK_BOX(start_card), lbl_tip_start);
    
    add_footer(start_card);

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

    GtkWidget *btn_end = gtk_button_new_with_label("End Battle");
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