#include <glib.h>
#include "../src/rock_paper_scissors/rps_logic.h"

static void test_rps_draw(void) {
    g_assert_cmpint(rps_evaluate_round(1, 1), ==, 0); // Rock vs Rock
    g_assert_cmpint(rps_evaluate_round(2, 2), ==, 0); // Paper vs Paper
    g_assert_cmpint(rps_evaluate_round(3, 3), ==, 0); // Scissors vs Scissors
}

static void test_rps_player_win(void) {
    g_assert_cmpint(rps_evaluate_round(1, 3), ==, 1); // Rock vs Scissors
    g_assert_cmpint(rps_evaluate_round(2, 1), ==, 1); // Paper vs Rock
    g_assert_cmpint(rps_evaluate_round(3, 2), ==, 1); // Scissors vs Paper
}

static void test_rps_computer_win(void) {
    g_assert_cmpint(rps_evaluate_round(3, 1), ==, 2); // Scissors vs Rock
    g_assert_cmpint(rps_evaluate_round(1, 2), ==, 2); // Rock vs Paper
    g_assert_cmpint(rps_evaluate_round(2, 3), ==, 2); // Paper vs Scissors
}

int main(int argc, char **argv) {
    g_test_init(&argc, &argv, NULL);
    g_test_add_func("/rps_logic/draw", test_rps_draw);
    g_test_add_func("/rps_logic/player_win", test_rps_player_win);
    g_test_add_func("/rps_logic/computer_win", test_rps_computer_win);

    return g_test_run();
}
