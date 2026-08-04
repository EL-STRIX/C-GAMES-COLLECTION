#include <gtk/gtk.h>
#include <glib.h>

void switch_to_launcher(void);
void switch_to_launcher(void) {}

#include "../src/tic_tac_toe/main.c"

static void test_reset_board_logic(void) {
    TttAppData app = {0};

    // Mutate the board to something else
    app.game.board[0][0] = 'X';
    app.game.board[2][2] = 'O';

    app.game.current_round = 1;

    reset_board_logic(&app);

    g_assert_cmpint(app.game.board[0][0], ==, '1');
    g_assert_cmpint(app.game.board[0][1], ==, '2');
    g_assert_cmpint(app.game.board[0][2], ==, '3');
    g_assert_cmpint(app.game.board[1][0], ==, '4');
    g_assert_cmpint(app.game.board[1][1], ==, '5');
    g_assert_cmpint(app.game.board[1][2], ==, '6');
    g_assert_cmpint(app.game.board[2][0], ==, '7');
    g_assert_cmpint(app.game.board[2][1], ==, '8');
    g_assert_cmpint(app.game.board[2][2], ==, '9');

    g_assert_cmpint(app.game.current_player, ==, 1);

    app.game.current_round = 2;
    reset_board_logic(&app);
    g_assert_cmpint(app.game.current_player, ==, 2);
}

int main(int argc, char *argv[]) {
    // We need gtk_init() because main.c includes gtk widgets
    // but the test exercises only the logic function
    gtk_init();

    g_test_init(&argc, &argv, NULL);
    g_test_add_func("/tic_tac_toe/reset_board_logic", test_reset_board_logic);
    return g_test_run();
}
