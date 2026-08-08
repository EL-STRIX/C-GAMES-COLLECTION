#include <glib.h>
#include <gtk/gtk.h>
#include "../src/tic_tac_toe/main.c"

// Mock the required external function
void switch_to_launcher(void) {}

static void test_check_draw(void) {
    TttAppData app;
    memset(&app, 0, sizeof(app));

    // Case 1: Partially filled board (not a draw)
    reset_board_logic(&app);
    g_assert_cmpint(check_draw(&app), ==, 0);

    // Case 2: Fully filled board (draw)
    for (int r = 0; r < 3; r++) {
        for (int c = 0; c < 3; c++) {
            app.game.board[r][c] = ((r + c) % 2 == 0) ? 'X' : 'O';
        }
    }
    g_assert_cmpint(check_draw(&app), ==, 1);

    // Case 3: Board with one empty spot (not a draw)
    app.game.board[2][2] = '9';
    g_assert_cmpint(check_draw(&app), ==, 0);
}

static void test_reset_board_logic(void) {
    TttAppData app;
    memset(&app, 0, sizeof(app));

    // Round 1
    app.game.current_round = 1;
    reset_board_logic(&app);

    g_assert_cmpint(app.game.board[0][0], ==, '1');
    g_assert_cmpint(app.game.board[2][2], ==, '9');
    g_assert_cmpint(app.game.current_player, ==, 1);
    g_assert_cmpint(app.game.game_over, ==, 0);

    // Round 2
    app.game.current_round = 2;
    reset_board_logic(&app);
    g_assert_cmpint(app.game.current_player, ==, 2);
}

int main(int argc, char **argv) {
    g_test_init(&argc, &argv, NULL);
    g_test_add_func("/tic_tac_toe/reset_board_logic", test_reset_board_logic);
    g_test_add_func("/tic_tac_toe/check_draw", test_check_draw);
    return g_test_run();
}
