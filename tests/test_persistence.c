#include <stdio.h>
#include <string.h>
#include <glib.h>
#include <glib/gstdio.h>
#ifdef _WIN32
#include <windows.h>
#endif

#include "../src/common/persistence.h"

/* -----------------------------------------------------------------------
 * test_settings
 *
 * BUG FIXED: The old test called remove("data/settings.ini") which would
 * delete the REAL user settings file. Tests should use a temp directory
 * or a dedicated test key so they don't destroy production data.
 *
 * The current approach uses a temp directory setup in main().
 * ----------------------------------------------------------------------- */
static void test_settings(void) {
    /* Write test data */
    save_global_settings("TestPlayer", 2);

    char out_name[50] = {0};
    int  out_theme    = 0;
    load_global_settings(out_name, sizeof(out_name), &out_theme);

    g_assert_cmpstr(out_name, ==, "TestPlayer");
    g_assert_cmpint(out_theme, ==, 2);
}

/* -----------------------------------------------------------------------
 * test_scores
 *
 * BUG FIXED: Used "test_game" as the game ID so it writes to
 * data/test_game_score.ini — a file that definitely should NOT exist in
 * production. The cleanup remove() is therefore safe for this file only.
 * ----------------------------------------------------------------------- */
static void test_scores(void) {
    /* is_lower_better = 0 (higher is better) */
    save_score("test_game", "Alice", 100, 0);

    char top_player[50] = {0};
    int  top_score      = load_top_score("test_game", top_player, sizeof(top_player));
    g_assert_cmpstr(top_player, ==, "Alice");
    g_assert_cmpint(top_score,  ==, 100);

    /* Bob scores lower — should NOT displace Alice */
    save_score("test_game", "Bob", 50, 0);
    top_score = load_top_score("test_game", top_player, sizeof(top_player));
    g_assert_cmpstr(top_player, ==, "Alice");
    g_assert_cmpint(top_score,  ==, 100);

    /* Charlie scores higher — should save */
    save_score("test_game", "Charlie", 200, 0);
    top_score = load_top_score("test_game", top_player, sizeof(top_player));
    g_assert_cmpstr(top_player, ==, "Charlie");
    g_assert_cmpint(top_score,  ==, 200);
}

/* -----------------------------------------------------------------------
 * test_scores_lower_better
 * NEW: Verify is_lower_better logic (Number Guessing uses this).
 * ----------------------------------------------------------------------- */
static void test_scores_lower_better(void) {
    /* is_lower_better = 1 (fewer guesses is better) */
    save_score("test_game_lb", "Alice", 10, 1);

    char top_player[50] = {0};
    int  top_score      = load_top_score("test_game_lb", top_player, sizeof(top_player));
    g_assert_cmpstr(top_player, ==, "Alice");
    g_assert_cmpint(top_score,  ==, 10);

    /* Bob scores higher (worse) — Alice should still win */
    save_score("test_game_lb", "Bob", 15, 1);
    top_score = load_top_score("test_game_lb", top_player, sizeof(top_player));
    g_assert_cmpstr(top_player, ==, "Alice");
    g_assert_cmpint(top_score,  ==, 10);

    /* Charlie scores lower (better) — should save */
    save_score("test_game_lb", "Charlie", 3, 1);
    top_score = load_top_score("test_game_lb", top_player, sizeof(top_player));
    g_assert_cmpstr(top_player, ==, "Charlie");
    g_assert_cmpint(top_score,  ==, 3);
}

/* -----------------------------------------------------------------------
 * test_load_missing_score
 * NEW: Ensure load_top_score returns -1 when no file exists.
 * ----------------------------------------------------------------------- */
static void test_load_missing_score(void) {
    char player[50] = {0};
    int score = load_top_score("definitely_missing_game", player, sizeof(player));
    g_assert_cmpint(score, ==, -1);
}


/* -----------------------------------------------------------------------
 * test_save_score_invalid_path
 * NEW: Verify missing edge cases for path traversal in save_score and load_top_score.
 * ----------------------------------------------------------------------- */
static void test_save_score_invalid_path(void) {
    g_test_expect_message(G_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "*Security violation*");
    save_score("../hacked_game", "Hacker", 9999, 0);
    g_test_assert_expected_messages();

    g_test_expect_message(G_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "*Security violation*");
    char player[50] = {0};
    int score = load_top_score("../hacked_game", player, sizeof(player));
    g_test_assert_expected_messages();

    g_assert_cmpint(score, ==, -1);
}

/* -----------------------------------------------------------------------
 * test_theme_preservation
 * NEW: Ensure save_global_settings(name, -1) preserves the existing theme.
 * ----------------------------------------------------------------------- */
static void test_theme_preservation(void) {
    /* Save a known theme */
    save_global_settings("ThemeTestPlayer", 2);

    /* Now update only the name (pass -1 for theme) */
    save_global_settings("UpdatedName", -1);

    char out_name[50] = {0};
    int  out_theme    = 0;
    load_global_settings(out_name, sizeof(out_name), &out_theme);

    g_assert_cmpstr(out_name,  ==, "UpdatedName");
    g_assert_cmpint(out_theme, ==, 2); /* theme must be preserved */
}


/* -----------------------------------------------------------------------
 * test_load_score_path_traversal
 * NEW: Ensure load_top_score rejects game_name with path traversal chars.
 * ----------------------------------------------------------------------- */
static void test_load_score_path_traversal(void) {
    char player[50] = {0};

    /* Expect the g_warning from the path traversal check */
    g_test_expect_message(G_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "*Security violation: path traversal detected*");
    int score = load_top_score("../test_game", player, sizeof(player));
    g_assert_cmpint(score, ==, -1);
    g_test_assert_expected_messages();

    g_test_expect_message(G_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "*Security violation: path traversal detected*");
    score = load_top_score("test\\game", player, sizeof(player));
    g_assert_cmpint(score, ==, -1);
    g_test_assert_expected_messages();

    g_test_expect_message(G_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "*Security violation: path traversal detected*");
    score = load_top_score("test/game", player, sizeof(player));
    g_assert_cmpint(score, ==, -1);
    g_test_assert_expected_messages();

    g_test_expect_message(G_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "*Security violation: path traversal detected*");
    score = load_top_score("test.game", player, sizeof(player));
    g_assert_cmpint(score, ==, -1);
    g_test_assert_expected_messages();
}

/* -----------------------------------------------------------------------
 * test_load_score_null_buffer
 * NEW: Ensure load_top_score handles NULL out_player_name and/or out_size=0.
 * ----------------------------------------------------------------------- */
static void test_load_score_null_buffer(void) {
    /* First, save a valid score so there's something to read */
    save_score("test_game_null", "Alice", 100, 0);

    /* Call load_top_score with NULL buffer */
    int score = load_top_score("test_game_null", NULL, 0);
    g_assert_cmpint(score, ==, 100);
}

/* -----------------------------------------------------------------------
 * test_secure_random_int_range
 * NEW: Verify behavior of get_secure_random_int_range.
 * ----------------------------------------------------------------------- */
static void test_secure_random_int_range(void) {
    /* Test basic positive range */
    for (int i = 0; i < 100; i++) {
        int result = get_secure_random_int_range(5, 10);
        g_assert_cmpint(result, >=, 5);
        g_assert_cmpint(result, <=, 10);
    }

    /* Test negative range */
    for (int i = 0; i < 100; i++) {
        int result = get_secure_random_int_range(-10, -5);
        g_assert_cmpint(result, >=, -10);
        g_assert_cmpint(result, <=, -5);
    }

    /* Test mixed range */
    for (int i = 0; i < 100; i++) {
        int result = get_secure_random_int_range(-5, 5);
        g_assert_cmpint(result, >=, -5);
        g_assert_cmpint(result, <=, 5);
    }

    /* Test edge case: begin == end */
    for (int i = 0; i < 10; i++) {
        int result = get_secure_random_int_range(42, 42);
        g_assert_cmpint(result, ==, 42);
    }

    /* Test inverted range (begin > end) */
    for (int i = 0; i < 100; i++) {
        int result = get_secure_random_int_range(10, 5);
        g_assert_cmpint(result, >=, 5);
        g_assert_cmpint(result, <=, 10);
    }
}

int main(int argc, char **argv) {
    g_test_init(&argc, &argv, NULL);

    /* Sandbox the data directory so tests don't overwrite production data */
    gchar *tmp_dir = g_dir_make_tmp("test_persistence_XXXXXX", NULL);
    gchar *tmp_data_dir = g_build_filename(tmp_dir, "data", NULL);
    g_mkdir(tmp_data_dir, 0700);

    /* Tell persistence.c to use our temporary data directory */
    g_setenv("PERSISTENCE_TEST_DATA_DIR", tmp_data_dir, TRUE);

    g_test_add_func("/persistence/settings",          test_settings);
    g_test_add_func("/persistence/load_score_path_traversal", test_load_score_path_traversal);
    g_test_add_func("/persistence/load_score_null_buffer", test_load_score_null_buffer);
    g_test_add_func("/persistence/scores",            test_scores);
    g_test_add_func("/persistence/scores_lower_better", test_scores_lower_better);
    g_test_add_func("/persistence/load_missing_score",  test_load_missing_score);
    g_test_add_func("/persistence/theme_preservation",  test_theme_preservation);
    g_test_add_func("/persistence/save_score_invalid_path", test_save_score_invalid_path);
    g_test_add_func("/persistence/secure_random_int_range", test_secure_random_int_range);

    int result = g_test_run();

    /* Clean up the temporary test environment */
    gchar *settings_file = g_build_filename(tmp_data_dir, "settings.ini", NULL);
    gchar *score_file1 = g_build_filename(tmp_data_dir, "test_game_score.ini", NULL);
    gchar *score_file2 = g_build_filename(tmp_data_dir, "test_game_lb_score.ini", NULL);
    gchar *score_file3 = g_build_filename(tmp_data_dir, "test_game_null_score.ini", NULL);

    g_remove(settings_file);
    g_remove(score_file1);
    g_remove(score_file2);
    g_remove(score_file3);
    g_rmdir(tmp_data_dir);
    g_rmdir(tmp_dir);

    g_free(settings_file);
    g_free(score_file1);
    g_free(score_file2);
    g_free(score_file3);
    g_free(tmp_data_dir);
    g_free(tmp_dir);

    return result;
}
