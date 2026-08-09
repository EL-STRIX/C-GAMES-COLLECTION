#include <glib.h>
#include <gtk/gtk.h>
#include <string.h>

// Control computer choice for deterministic testing
static int mock_computer_choice = 1; // Snake by default

// We need to define get_secure_random_int_range to return mock_computer_choice
// before including main.c. Note that main.c does not include persistence.c directly,
// it might include persistence.h. Let's provide an override macro.

#define get_secure_random_int_range mock_get_secure_random_int_range

int mock_get_secure_random_int_range(int begin, int end);
void switch_to_launcher(void);

int mock_get_secure_random_int_range(int begin, int end) {
    (void)begin;
    (void)end;
    return mock_computer_choice;
}

// Mock other external functions that might be used
void switch_to_launcher(void) {}

// Ensure the implementation is included
#include "../src/snake_gun_water/main.c"

static void test_sgw_process_round_win(void) {
    SgwAppData data;
    memset(&data, 0, sizeof(SgwAppData));

    // Set up mock widgets
    data.feedback_label = gtk_label_new("");
    data.result_label = gtk_label_new("");
    data.score_label = gtk_label_new("");
    data.choices_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    data.next_round_btn = gtk_button_new();

    data.player_score = 0;
    data.computer_score = 0;
    data.current_round = 1;
    strcpy(data.player_name, "TestPlayer");

    // Test Win: Player Snake vs Computer Water
    mock_computer_choice = CHOICE_WATER;
    sgw_process_round(&data, CHOICE_SNAKE);

    g_assert_cmpint(data.player_score, ==, 1);
    g_assert_cmpint(data.computer_score, ==, 0);

    // Player Gun vs Computer Snake
    mock_computer_choice = CHOICE_SNAKE;
    sgw_process_round(&data, CHOICE_GUN);

    g_assert_cmpint(data.player_score, ==, 2);
    g_assert_cmpint(data.computer_score, ==, 0);

    // Player Water vs Computer Gun
    mock_computer_choice = CHOICE_GUN;
    sgw_process_round(&data, CHOICE_WATER);

    g_assert_cmpint(data.player_score, ==, 3);
    g_assert_cmpint(data.computer_score, ==, 0);
}

static void test_sgw_process_round_lose(void) {
    SgwAppData data;
    memset(&data, 0, sizeof(SgwAppData));

    // Set up mock widgets
    data.feedback_label = gtk_label_new("");
    data.result_label = gtk_label_new("");
    data.score_label = gtk_label_new("");
    data.choices_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    data.next_round_btn = gtk_button_new();

    data.player_score = 0;
    data.computer_score = 0;
    data.current_round = 1;
    strcpy(data.player_name, "TestPlayer");

    // Test Lose: Player Snake vs Computer Gun
    mock_computer_choice = CHOICE_GUN;
    sgw_process_round(&data, CHOICE_SNAKE);

    g_assert_cmpint(data.player_score, ==, 0);
    g_assert_cmpint(data.computer_score, ==, 1);
}

static void test_sgw_process_round_draw(void) {
    SgwAppData data;
    memset(&data, 0, sizeof(SgwAppData));

    // Set up mock widgets
    data.feedback_label = gtk_label_new("");
    data.result_label = gtk_label_new("");
    data.score_label = gtk_label_new("");
    data.choices_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    data.next_round_btn = gtk_button_new();

    data.player_score = 0;
    data.computer_score = 0;
    data.current_round = 1;
    strcpy(data.player_name, "TestPlayer");

    // Test Draw: Player Snake vs Computer Snake
    mock_computer_choice = CHOICE_SNAKE;
    sgw_process_round(&data, CHOICE_SNAKE);

    g_assert_cmpint(data.player_score, ==, 0);
    g_assert_cmpint(data.computer_score, ==, 0);
}

int main(int argc, char **argv) {
    gtk_init(); // Necessary for tests interacting with GTK widgets
    g_test_init(&argc, &argv, NULL);

    g_test_add_func("/snake_gun_water/process_round_win", test_sgw_process_round_win);
    g_test_add_func("/snake_gun_water/process_round_lose", test_sgw_process_round_lose);
    g_test_add_func("/snake_gun_water/process_round_draw", test_sgw_process_round_draw);

    return g_test_run();
}
