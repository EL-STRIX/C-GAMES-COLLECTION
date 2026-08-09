#include <glib.h>
#include <gtk/gtk.h>
#include <string.h>

// Control computer choice for deterministic testing
static int mock_computer_choice = 1; // Rock by default

#define get_secure_random_int_range mock_get_secure_random_int_range

int mock_get_secure_random_int_range(int begin, int end) {
    (void)begin;
    (void)end;
    return mock_computer_choice;
}

// Mock external function
void switch_to_launcher(void) {}

// Include the source file
#include "../src/rock_paper_scissors/main.c"

static void test_rps_process_round_win(void) {
    RpsAppData data;
    memset(&data, 0, sizeof(RpsAppData));

    // Mock widgets
    data.feedback_label = gtk_label_new("");
    data.result_label = gtk_label_new("");
    data.score_label = gtk_label_new("");
    data.round_label = gtk_label_new("");
    data.choices_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    data.next_round_btn = gtk_button_new();

    data.player_score = 0;
    data.computer_score = 0;
    data.current_round = 1;
    strcpy(data.player_name, "TestPlayer");

    // Rock vs Scissors
    mock_computer_choice = CHOICE_SCISSORS;
    rps_process_round(&data, CHOICE_ROCK);
    g_assert_cmpint(data.player_score, ==, 1);
    g_assert_cmpint(data.computer_score, ==, 0);

    // Paper vs Rock
    mock_computer_choice = CHOICE_ROCK;
    rps_process_round(&data, CHOICE_PAPER);
    g_assert_cmpint(data.player_score, ==, 2);
    g_assert_cmpint(data.computer_score, ==, 0);

    // Scissors vs Paper
    mock_computer_choice = CHOICE_PAPER;
    rps_process_round(&data, CHOICE_SCISSORS);
    g_assert_cmpint(data.player_score, ==, 3);
    g_assert_cmpint(data.computer_score, ==, 0);
}

static void test_rps_process_round_lose(void) {
    RpsAppData data;
    memset(&data, 0, sizeof(RpsAppData));

    // Mock widgets
    data.feedback_label = gtk_label_new("");
    data.result_label = gtk_label_new("");
    data.score_label = gtk_label_new("");
    data.round_label = gtk_label_new("");
    data.choices_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    data.next_round_btn = gtk_button_new();

    data.player_score = 0;
    data.computer_score = 0;
    data.current_round = 1;
    strcpy(data.player_name, "TestPlayer");

    // Rock vs Paper
    mock_computer_choice = CHOICE_PAPER;
    rps_process_round(&data, CHOICE_ROCK);
    g_assert_cmpint(data.player_score, ==, 0);
    g_assert_cmpint(data.computer_score, ==, 1);

    // Paper vs Scissors
    mock_computer_choice = CHOICE_SCISSORS;
    rps_process_round(&data, CHOICE_PAPER);
    g_assert_cmpint(data.player_score, ==, 0);
    g_assert_cmpint(data.computer_score, ==, 2);

    // Scissors vs Rock
    mock_computer_choice = CHOICE_ROCK;
    rps_process_round(&data, CHOICE_SCISSORS);
    g_assert_cmpint(data.player_score, ==, 0);
    g_assert_cmpint(data.computer_score, ==, 3);
}

static void test_rps_process_round_draw(void) {
    RpsAppData data;
    memset(&data, 0, sizeof(RpsAppData));

    // Mock widgets
    data.feedback_label = gtk_label_new("");
    data.result_label = gtk_label_new("");
    data.score_label = gtk_label_new("");
    data.round_label = gtk_label_new("");
    data.choices_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    data.next_round_btn = gtk_button_new();

    data.player_score = 0;
    data.computer_score = 0;
    data.current_round = 1;
    strcpy(data.player_name, "TestPlayer");

    // Rock vs Rock
    mock_computer_choice = CHOICE_ROCK;
    rps_process_round(&data, CHOICE_ROCK);
    g_assert_cmpint(data.player_score, ==, 0);
    g_assert_cmpint(data.computer_score, ==, 0);

    // Paper vs Paper
    mock_computer_choice = CHOICE_PAPER;
    rps_process_round(&data, CHOICE_PAPER);
    g_assert_cmpint(data.player_score, ==, 0);
    g_assert_cmpint(data.computer_score, ==, 0);

    // Scissors vs Scissors
    mock_computer_choice = CHOICE_SCISSORS;
    rps_process_round(&data, CHOICE_SCISSORS);
    g_assert_cmpint(data.player_score, ==, 0);
    g_assert_cmpint(data.computer_score, ==, 0);
}

int main(int argc, char **argv) {
    gtk_init(); // Necessary for tests interacting with GTK widgets
    g_test_init(&argc, &argv, NULL);

    g_test_add_func("/rock_paper_scissors/process_round_win", test_rps_process_round_win);
    g_test_add_func("/rock_paper_scissors/process_round_lose", test_rps_process_round_lose);
    g_test_add_func("/rock_paper_scissors/process_round_draw", test_rps_process_round_draw);

    return g_test_run();
}
