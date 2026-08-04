#include <glib.h>
#include <gtk/gtk.h>
#include "../src/tic_tac_toe/main.c"

// Mock the missing symbol for switch_to_launcher to allow compilation
void switch_to_launcher(void) {}

static void test_check_draw_empty(void) {
    TttAppData app = {0};
    reset_board_logic(&app); // Sets 1,2,3,4,5,6,7,8,9
    g_assert_cmpint(check_draw(&app), ==, 0);
}

static void test_check_draw_full_no_winner(void) {
    TttAppData app = {0};
    reset_board_logic(&app);
    // Board: X O X, O X O, O X O (no winner, full)
    app.game.board[0][0] = 'X'; app.game.board[0][1] = 'O'; app.game.board[0][2] = 'X';
    app.game.board[1][0] = 'O'; app.game.board[1][1] = 'X'; app.game.board[1][2] = 'O';
    app.game.board[2][0] = 'O'; app.game.board[2][1] = 'X'; app.game.board[2][2] = 'O';
    g_assert_cmpint(check_draw(&app), ==, 1);
}

static void test_check_draw_partial(void) {
    TttAppData app = {0};
    reset_board_logic(&app);
    // Board: X O X, O 5 6, 7 8 9
    app.game.board[0][0] = 'X'; app.game.board[0][1] = 'O'; app.game.board[0][2] = 'X';
    app.game.board[1][0] = 'O';
    g_assert_cmpint(check_draw(&app), ==, 0);
}

int main(int argc, char **argv) {
    g_test_init(&argc, &argv, NULL);

    // memory: omit gtk_init() if the test suite only exercises early-return paths that do not actually reach GTK-specific logic
    g_test_add_func("/tic_tac_toe/check_draw_empty", test_check_draw_empty);
    g_test_add_func("/tic_tac_toe/check_draw_full_no_winner", test_check_draw_full_no_winner);
    g_test_add_func("/tic_tac_toe/check_draw_partial", test_check_draw_partial);

    return g_test_run();
}
