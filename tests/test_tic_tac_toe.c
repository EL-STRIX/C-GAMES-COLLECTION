#include <glib.h>
#include <gtk/gtk.h>
#include <string.h>

void switch_to_launcher(void);
void switch_to_launcher(void) {}

#include "../src/tic_tac_toe/main.c"

static void test_check_winner_horizontal(void) {
    TttAppData app;
    memset(&app, 0, sizeof(TttAppData));

    for (int i = 0; i < 3; i++) {
        reset_board_logic(&app);
        app.game.board[i][0] = 'X';
        app.game.board[i][1] = 'X';
        app.game.board[i][2] = 'X';
        g_assert_cmpint(check_winner(&app), ==, 1);

        reset_board_logic(&app);
        app.game.board[i][0] = 'O';
        app.game.board[i][1] = 'O';
        app.game.board[i][2] = 'O';
        g_assert_cmpint(check_winner(&app), ==, 1);
    }
}

static void test_check_winner_vertical(void) {
    TttAppData app;
    memset(&app, 0, sizeof(TttAppData));

    for (int i = 0; i < 3; i++) {
        reset_board_logic(&app);
        app.game.board[0][i] = 'X';
        app.game.board[1][i] = 'X';
        app.game.board[2][i] = 'X';
        g_assert_cmpint(check_winner(&app), ==, 1);

        reset_board_logic(&app);
        app.game.board[0][i] = 'O';
        app.game.board[1][i] = 'O';
        app.game.board[2][i] = 'O';
        g_assert_cmpint(check_winner(&app), ==, 1);
    }
}

static void test_check_winner_diagonal(void) {
    TttAppData app;
    memset(&app, 0, sizeof(TttAppData));

    // Main diagonal
    reset_board_logic(&app);
    app.game.board[0][0] = 'X';
    app.game.board[1][1] = 'X';
    app.game.board[2][2] = 'X';
    g_assert_cmpint(check_winner(&app), ==, 1);

    // Anti-diagonal
    reset_board_logic(&app);
    app.game.board[0][2] = 'O';
    app.game.board[1][1] = 'O';
    app.game.board[2][0] = 'O';
    g_assert_cmpint(check_winner(&app), ==, 1);
}

static void test_check_winner_no_winner(void) {
    TttAppData app;
    memset(&app, 0, sizeof(TttAppData));

    // Initial board state (1 to 9)
    reset_board_logic(&app);
    g_assert_cmpint(check_winner(&app), ==, 0);

    // Mix of X and O with no winner
    app.game.board[0][0] = 'X'; app.game.board[0][1] = 'O'; app.game.board[0][2] = 'X';
    app.game.board[1][0] = 'X'; app.game.board[1][1] = 'O'; app.game.board[1][2] = 'O';
    app.game.board[2][0] = 'O'; app.game.board[2][1] = 'X'; app.game.board[2][2] = 'X';
    g_assert_cmpint(check_winner(&app), ==, 0);
}

int main(int argc, char **argv) {
    g_test_init(&argc, &argv, NULL);
    g_test_add_func("/tic_tac_toe/check_winner/horizontal", test_check_winner_horizontal);
    g_test_add_func("/tic_tac_toe/check_winner/vertical", test_check_winner_vertical);
    g_test_add_func("/tic_tac_toe/check_winner/diagonal", test_check_winner_diagonal);
    g_test_add_func("/tic_tac_toe/check_winner/no_winner", test_check_winner_no_winner);
    return g_test_run();
}
