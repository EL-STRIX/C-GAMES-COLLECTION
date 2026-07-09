#include <gtk/gtk.h>
#include <string.h>
#include "../common/persistence.h"
#include "../common/constants.h"
#include "../common/ui_utils.h"
#include "../common/games.h"

#ifdef _WIN32
#include <windows.h>
#endif

// ============================================================
// GAME STATE
// ============================================================

typedef struct {
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

// TttAppData encapsulates the application's runtime state and widget references,
// eliminating globals and ensuring memory safety across callback boundaries.
typedef struct {
    GameState game;
    GtkWidget *window;
    GtkWidget *stack;
    GtkWidget *entry_p1;
    GtkWidget *entry_p2;
    GtkWidget *label_score_p1;
    GtkWidget *label_score_p2;
    GtkWidget *buttons[3][3];
    GtkWidget *result_title;
    GtkWidget *result_subtitle;
    GtkWidget *result_score_label;
    GtkWidget *lbl_start_error;
} TttAppData;

// ============================================================
// GAME LOGIC
// ============================================================

void init_game_state(TttAppData *app)
{
    app->game.current_round = 1;
    app->game.score1 = 0;
    app->game.score2 = 0;
    for (int i = 0; i < 3; i++)
        app->game.round_winners[i] = -1;
}

void reset_board_logic(TttAppData *app)
{
    char num = '1';
    for (int r = 0; r < 3; ++r)
    {
        for (int c = 0; c < 3; ++c)
        {
            app->game.board[r][c] = num++;
        }
    }
    app->game.current_player = (app->game.current_round % 2 != 0) ? 1 : 2;
    app->game.game_over = 0;
}

int check_winner(TttAppData *app)
{
    for (int i = 0; i < 3; i++)
    {
        if (app->game.board[i][0] == app->game.board[i][1] && app->game.board[i][1] == app->game.board[i][2]) return 1;
        if (app->game.board[0][i] == app->game.board[1][i] && app->game.board[1][i] == app->game.board[2][i]) return 1;
    }
    if (app->game.board[0][0] == app->game.board[1][1] && app->game.board[1][1] == app->game.board[2][2]) return 1;
    if (app->game.board[0][2] == app->game.board[1][1] && app->game.board[1][1] == app->game.board[2][0]) return 1;
    return 0;
}

int check_draw(TttAppData *app)
{
    for (int r = 0; r < 3; r++)
        for (int c = 0; c < 3; c++)
            if (app->game.board[r][c] != 'X' && app->game.board[r][c] != 'O') return 0;
    return 1;
}

// ============================================================
// UI UPDATES
// ============================================================

void update_ui_board(TttAppData *app)
{
    char s1[100], s2[100];
    snprintf(s1, sizeof(s1), "%s: %d", app->game.name1, app->game.score1);
    snprintf(s2, sizeof(s2), "%s: %d", app->game.name2, app->game.score2);
    gtk_label_set_text(GTK_LABEL(app->label_score_p1), s1);
    gtk_label_set_text(GTK_LABEL(app->label_score_p2), s2);

    for (int r = 0; r < 3; r++)
    {
        for (int c = 0; c < 3; c++)
        {
            char val = app->game.board[r][c];
            GtkWidget *btn = app->buttons[r][c];

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

void show_result_screen(TttAppData *app)
{
    gtk_label_set_text(GTK_LABEL(app->result_title), "GAME OVER");

    char result_txt[100];
    if (app->game.score1 > app->game.score2) {
        snprintf(result_txt, 100, "VICTORY!\n%s won the match.", app->game.name1);
        gtk_widget_remove_css_class(app->result_subtitle, "error");
        gtk_widget_add_css_class(app->result_subtitle, "success");
    } else if (app->game.score2 > app->game.score1) {
        snprintf(result_txt, 100, "VICTORY!\n%s won the match.", app->game.name2);
        gtk_widget_remove_css_class(app->result_subtitle, "success");
        gtk_widget_add_css_class(app->result_subtitle, "error");
    } else {
        snprintf(result_txt, 100, "DRAW!\nNo clear winner.");
        gtk_widget_remove_css_class(app->result_subtitle, "success");
        gtk_widget_remove_css_class(app->result_subtitle, "error");
        gtk_widget_add_css_class(app->result_subtitle, "warning");
    }

    gtk_label_set_text(GTK_LABEL(app->result_subtitle), result_txt);

    char best_player[50];
    int best_score = load_top_score("ttt_gui", best_player, sizeof(best_player));
    char score_txt[200];
    if (best_score != -1) {
        snprintf(score_txt, 200, "Final Score: %d - %d\nAll-Time Best: %s (%d wins)", app->game.score1, app->game.score2, best_player, best_score);
    } else {
        snprintf(score_txt, 200, "Final Score: %d - %d", app->game.score1, app->game.score2);
    }
    gtk_label_set_text(GTK_LABEL(app->result_score_label), score_txt);

    gtk_stack_set_visible_child_name(GTK_STACK(app->stack), "result_page");
}

void handle_round_end(TttAppData *app, int winner_id)
{
    app->game.game_over = 1;
    if (winner_id == 1) {
        app->game.score1++;
        save_score("ttt_gui", app->game.name1, app->game.score1, 0);
    } else if (winner_id == 2) {
        app->game.score2++;
        save_score("ttt_gui", app->game.name2, app->game.score2, 0);
    }

    app->game.round_winners[app->game.current_round - 1] = winner_id;

    if (app->game.current_round >= 3) {
        show_result_screen(app);
    } else {
        app->game.current_round++;
        reset_board_logic(app);
        update_ui_board(app);
    }
}

// ============================================================
// SIGNALS
// ============================================================

void on_cell_clicked(GtkWidget *widget, gpointer data)
{
    TttAppData *app = (TttAppData *)data;
    if (app->game.game_over) return;
    int id = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(widget), "cell_id"));
    int r = id / 3;
    int c = id % 3;

    if (app->game.board[r][c] == 'X' || app->game.board[r][c] == 'O') return;

    app->game.board[r][c] = (app->game.current_player == 1) ? 'X' : 'O';

    if (check_winner(app)) {
        update_ui_board(app);
        handle_round_end(app, app->game.current_player);
    } else if (check_draw(app)) {
        update_ui_board(app);
        handle_round_end(app, 0);
    } else {
        app->game.current_player = (app->game.current_player == 1) ? 2 : 1;
        update_ui_board(app);
    }
}

void on_start_clicked(GtkWidget *widget, gpointer data)
{
    (void)widget;
    TttAppData *app = (TttAppData *)data;
    const char *name = gtk_editable_get_text(GTK_EDITABLE(app->entry_p1));
    char *trimmed = g_strstrip(g_strdup(name));
    if (g_utf8_strlen(trimmed, -1) == 0) {
        gtk_label_set_text(GTK_LABEL(app->lbl_start_error), "Please enter your name to play!");
        g_free(trimmed);
        return;
    }
    gtk_label_set_text(GTK_LABEL(app->lbl_start_error), "");
    
    strncpy(app->game.name1, trimmed, sizeof(app->game.name1) - 1);
    g_free(trimmed);
    app->game.name1[sizeof(app->game.name1) - 1] = '\0';
    save_global_settings(app->game.name1, -1);
    
    snprintf(app->game.name2, sizeof(app->game.name2), "Guest");

    init_game_state(app);
    reset_board_logic(app);
    update_ui_board(app);
    
    gtk_stack_set_visible_child_name(GTK_STACK(app->stack), "game_page");
}

void on_reset_game_clicked(GtkWidget *widget, gpointer data)
{
    (void)widget;
    TttAppData *app = (TttAppData *)data;
    reset_board_logic(app);
    update_ui_board(app);
}

void ttt_on_play_again_clicked(GtkWidget *widget, gpointer data)
{
    (void)widget;
    TttAppData *app = (TttAppData *)data;
    init_game_state(app);
    reset_board_logic(app);
    update_ui_board(app);
    gtk_stack_set_visible_child_name(GTK_STACK(app->stack), "game_page");
}

// ============================================================
// UI NAVIGATION
// ============================================================

void on_header_back_clicked(GtkButton *btn, gpointer user_data)
{
    (void)btn;
    TttAppData *app = (TttAppData *)user_data;
    handle_header_back_clicked(app->window, app->stack, "game_page");
}

// ============================================================
// BUILD CARD HELPERS
// ============================================================



void add_footer(GtkWidget *box)
{
    GtkWidget *footer = gtk_label_new("Developed by SUJAY PAUL");
    gtk_widget_add_css_class(footer, "footer-credit");
    gtk_box_append(GTK_BOX(box), footer);
}

// ============================================================
// MAIN UI SETUP
// ============================================================

static void ttt_on_header_back_clicked(GtkButton *btn, gpointer user_data)
{
    (void)btn;
    (void)user_data;
    switch_to_launcher();
}

GtkWidget* ttt_create_ui(void)
{

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    TttAppData *app = g_new0(TttAppData, 1);


    GtkWidget *header = gtk_header_bar_new();
    gtk_header_bar_set_show_title_buttons(GTK_HEADER_BAR(header), TRUE);
    gtk_box_append(GTK_BOX(vbox), header);
    
    GtkWidget *title_lbl = gtk_label_new("Epic Tic Tac Toe Battle");
    gtk_widget_add_css_class(title_lbl, "header-title");
    gtk_header_bar_set_title_widget(GTK_HEADER_BAR(header), title_lbl);

    app->stack = gtk_stack_new();
    gtk_stack_set_transition_type(GTK_STACK(app->stack), GTK_STACK_TRANSITION_TYPE_CROSSFADE);
    // Global Overlay for the "Return to Main Menu" button
    GtkWidget *overlay = gtk_overlay_new();
    gtk_overlay_set_child(GTK_OVERLAY(overlay), app->stack);

    GtkWidget *btn_box_back = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    gtk_widget_set_halign(btn_box_back, GTK_ALIGN_CENTER);
    GtkWidget *back_icon = gtk_image_new_from_icon_name("go-previous-symbolic");
    GtkWidget *back_lbl = gtk_label_new("Main Menu");
    gtk_box_append(GTK_BOX(btn_box_back), back_icon);
    gtk_box_append(GTK_BOX(btn_box_back), back_lbl);
    
    GtkWidget *global_btn_back = gtk_button_new();
    gtk_button_set_child(GTK_BUTTON(global_btn_back), btn_box_back);
    gtk_widget_set_halign(global_btn_back, GTK_ALIGN_START);
    gtk_widget_set_valign(global_btn_back, GTK_ALIGN_START);
    gtk_widget_set_margin_top(global_btn_back, 15);
    gtk_widget_set_margin_end(global_btn_back, 15);
    gtk_widget_add_css_class(global_btn_back, "btn-secondary");
    g_signal_connect(global_btn_back, "clicked", G_CALLBACK(ttt_on_header_back_clicked), app);

    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), global_btn_back);

    gtk_box_append(GTK_BOX(vbox), overlay);

    // ============================================================
    // PAGE 1: START SCREEN
    // ============================================================
    GtkWidget *start_page_wrapper = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_halign(start_page_wrapper, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(start_page_wrapper, GTK_ALIGN_CENTER);

    GtkWidget *start_card = create_card_box();
    
    GtkWidget *lbl_title = gtk_label_new("≡ƒÄ« TIC TAC TOE");
    gtk_widget_add_css_class(lbl_title, "title-large");
    gtk_box_append(GTK_BOX(start_card), lbl_title);

    GtkWidget *lbl_subtitle = gtk_label_new("Welcome to the Arena");
    gtk_widget_add_css_class(lbl_subtitle, "subtitle");
    gtk_box_append(GTK_BOX(start_card), lbl_subtitle);

    GtkWidget *lbl_p1 = gtk_label_new("What is your name, Challenger?");
    gtk_widget_set_halign(lbl_p1, GTK_ALIGN_START);
    gtk_widget_add_css_class(lbl_p1, "subtitle");
    gtk_box_append(GTK_BOX(start_card), lbl_p1);

    app->entry_p1 = gtk_entry_new();
    gtk_widget_add_css_class(app->entry_p1, "styled-entry");
    gtk_entry_set_placeholder_text(GTK_ENTRY(app->entry_p1), "Type name here...");
    gtk_box_append(GTK_BOX(start_card), app->entry_p1);
    
    app->lbl_start_error = gtk_label_new("");
    gtk_widget_add_css_class(app->lbl_start_error, "error-msg");
    gtk_box_append(GTK_BOX(start_card), app->lbl_start_error);

    GtkWidget *btn_start = gtk_button_new_with_label("Start Battle");
    gtk_widget_add_css_class(btn_start, "btn-primary");
    g_signal_connect(btn_start, "clicked", G_CALLBACK(on_start_clicked), app);
    gtk_box_append(GTK_BOX(start_card), btn_start);

    gtk_box_append(GTK_BOX(start_page_wrapper), start_card);
    gtk_stack_add_named(GTK_STACK(app->stack), start_page_wrapper, "start_page");

    // ============================================================
    // PAGE 2: GAME BOARD
    // ============================================================
    GtkWidget *game_page_wrapper = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_halign(game_page_wrapper, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(game_page_wrapper, GTK_ALIGN_CENTER);

    GtkWidget *game_card = create_card_box();
    
    GtkWidget *lbl_round = gtk_label_new("Battle Arena");
    gtk_widget_add_css_class(lbl_round, "title-large");
    gtk_box_append(GTK_BOX(game_card), lbl_round);

    GtkWidget *box_scores = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 30);
    gtk_widget_set_halign(box_scores, GTK_ALIGN_CENTER);

    app->label_score_p1 = gtk_label_new("Player 1: 0");
    app->label_score_p2 = gtk_label_new("Player 2: 0");
    gtk_widget_add_css_class(app->label_score_p1, "score-info");
    gtk_widget_add_css_class(app->label_score_p2, "score-info");

    gtk_box_append(GTK_BOX(box_scores), app->label_score_p1);
    gtk_box_append(GTK_BOX(box_scores), app->label_score_p2);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_top(grid, 20);
    gtk_widget_set_margin_bottom(grid, 20);

    for (int i = 0; i < 9; i++) {
        int r = i / 3;
        int c = i % 3;
        app->buttons[r][c] = gtk_button_new_with_label("");
        gtk_widget_add_css_class(app->buttons[r][c], "grid-button");
        g_object_set_data(G_OBJECT(app->buttons[r][c]), "cell_id", GINT_TO_POINTER(i));
        g_signal_connect(app->buttons[r][c], "clicked", G_CALLBACK(on_cell_clicked), app);
        gtk_grid_attach(GTK_GRID(grid), app->buttons[r][c], c, r, 1, 1);
    }

    GtkWidget *btn_reset = gtk_button_new_with_label("Reset Game");
    gtk_widget_set_margin_top(btn_reset, 10);
    gtk_widget_add_css_class(btn_reset, "btn-primary");
    g_signal_connect(btn_reset, "clicked", G_CALLBACK(on_reset_game_clicked), app);
    
    GtkWidget *lbl_tip_game = gtk_label_new("≡ƒÆí Hint: Use your brain. It helps.");
    gtk_widget_set_margin_top(lbl_tip_game, 15);
    gtk_widget_add_css_class(lbl_tip_game, "subtitle");

    gtk_box_append(GTK_BOX(game_card), box_scores);
    gtk_box_append(GTK_BOX(game_card), grid);
    gtk_box_append(GTK_BOX(game_card), btn_reset);
    gtk_box_append(GTK_BOX(game_card), lbl_tip_game);
    
    add_footer(game_card); 

    gtk_box_append(GTK_BOX(game_page_wrapper), game_card);
    gtk_stack_add_named(GTK_STACK(app->stack), game_page_wrapper, "game_page");

    // ============================================================
    // PAGE 3: RESULT SCREEN
    // ============================================================
    GtkWidget *result_page_wrapper = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_halign(result_page_wrapper, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(result_page_wrapper, GTK_ALIGN_CENTER);

    GtkWidget *result_card = create_card_box();

    app->result_title = gtk_label_new("GAME OVER");
    gtk_widget_add_css_class(app->result_title, "title-large");

    app->result_subtitle = gtk_label_new("");
    gtk_label_set_justify(GTK_LABEL(app->result_subtitle), GTK_JUSTIFY_CENTER);
    gtk_widget_set_margin_bottom(app->result_subtitle, 10);
    gtk_widget_add_css_class(app->result_subtitle, "subtitle");

    app->result_score_label = gtk_label_new("Final Score: 0 - 0");
    gtk_widget_add_css_class(app->result_score_label, "score-info");

    GtkWidget *box_actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_set_halign(box_actions, GTK_ALIGN_CENTER);

    GtkWidget *btn_rematch = gtk_button_new_with_label("Play Again");
    gtk_widget_add_css_class(btn_rematch, "btn-primary");
    g_signal_connect(btn_rematch, "clicked", G_CALLBACK(ttt_on_play_again_clicked), app);

    gtk_box_append(GTK_BOX(box_actions), btn_rematch);

    gtk_box_append(GTK_BOX(result_card), app->result_title);
    gtk_box_append(GTK_BOX(result_card), app->result_subtitle);
    gtk_box_append(GTK_BOX(result_card), app->result_score_label);
    gtk_box_append(GTK_BOX(result_card), box_actions);
    add_footer(result_card);

    gtk_box_append(GTK_BOX(result_page_wrapper), result_card);
    gtk_stack_add_named(GTK_STACK(app->stack), result_page_wrapper, "result_page");
    
    int theme_id;
    char player_name[50];
    load_global_settings(player_name, sizeof(player_name), &theme_id);
    // apply_theme(theme_id); - Already applied globally
    
    /* Set default player name if available */
    if (strlen(player_name) > 0) {
        gtk_editable_set_text(GTK_EDITABLE(app->entry_p1), player_name);
    }
    
    if (strlen(player_name) > 0 && strcmp(player_name, "Player 1") != 0) {
        strncpy(app->game.name1, player_name, sizeof(app->game.name1) - 1);
        app->game.name1[sizeof(app->game.name1) - 1] = '\0';
        snprintf(app->game.name2, sizeof(app->game.name2), "Guest");
        init_game_state(app);
        gtk_stack_set_visible_child_name(GTK_STACK(app->stack), "game_page");
    } else {
        gtk_stack_set_visible_child_name(GTK_STACK(app->stack), "start_page");
    }

    return vbox;
}

