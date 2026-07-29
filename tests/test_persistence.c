#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>
#include <glib.h>
#ifdef _WIN32
#include <windows.h>
#endif

#include "../src/common/persistence.h"
#include "../src/common/ui_utils.h"

/**
 * Test-only stub: switch_to_launcher() is defined in launcher/main.c
 * which is not compiled into the test binary. Provide a no-op here
 * so ui_utils.c (confirm_exit_response) can link correctly.
 */
void switch_to_launcher(void) { /* no-op in test context */ }

/* -----------------------------------------------------------------------
 * test_settings
 *
 * BUG FIXED: The old test called remove("data/settings.ini") which would
 * delete the REAL user settings file. Tests should use a temp directory
 * or a dedicated test key so they don't destroy production data.
 *
 * The current approach saves/restores the original settings around the test.
 * ----------------------------------------------------------------------- */
static void test_settings(void) {
    /* Save whatever currently exists so we can restore it after the test */
    char original_name[50] = {0};
    int  original_theme    = 0;
    load_global_settings(original_name, sizeof(original_name), &original_theme);

    /* Write test data */
    save_global_settings("TestPlayer", 2);

    char out_name[50] = {0};
    int  out_theme    = 0;
    load_global_settings(out_name, sizeof(out_name), &out_theme);

    g_assert_cmpstr(out_name, ==, "TestPlayer");
    g_assert_cmpint(out_theme, ==, 2);

    /* Restore original data so the user's real settings aren't destroyed */
    if (original_name[0] != '\0') {
        save_global_settings(original_name, original_theme);
    }
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

    /* Cleanup test artefact only */
    remove("data/test_game_score.ini");
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

    remove("data/test_game_lb_score.ini");
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
 * test_load_css_path_traversal
 * NEW: Ensure load_css_from_file returns NULL and logs a warning on path traversal attempt.
 * ----------------------------------------------------------------------- */
static void test_load_css_path_traversal(void) {
    g_test_expect_message(G_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "*Security violation: path traversal detected in css filename '../style.css'*");
    GtkCssProvider* provider1 = load_css_from_file("../style.css");
    g_assert_null(provider1);
    g_test_assert_expected_messages();

    g_test_expect_message(G_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "*Security violation: path traversal detected in css filename 'subdir?style.css'*");
    GtkCssProvider* provider2 = load_css_from_file("subdir\\style.css");
    g_assert_null(provider2);
    g_test_assert_expected_messages();
}

/* -----------------------------------------------------------------------
 * test_theme_preservation
 * NEW: Ensure save_global_settings(name, -1) preserves the existing theme.
 * ----------------------------------------------------------------------- */
static void test_theme_preservation(void) {
    /* Save whatever currently exists so we can restore it after the test */
    char original_name[50] = {0};
    int  original_theme    = 0;
    load_global_settings(original_name, sizeof(original_name), &original_theme);

    /* Save a known theme */
    save_global_settings("ThemeTestPlayer", 2);

    /* Now update only the name (pass -1 for theme) */
    save_global_settings("UpdatedName", -1);

    char out_name[50] = {0};
    int  out_theme    = 0;
    load_global_settings(out_name, sizeof(out_name), &out_theme);

    g_assert_cmpstr(out_name,  ==, "UpdatedName");
    g_assert_cmpint(out_theme, ==, 2); /* theme must be preserved */

    /* Restore original so the user's real settings aren't destroyed */
    if (original_name[0] != '\0') {
        save_global_settings(original_name, original_theme);
    }
}

/* -----------------------------------------------------------------------
 * test_load_css_valid
 * NEW: Ensure load_css_from_file returns a valid GtkCssProvider.
 * ----------------------------------------------------------------------- */
static void test_load_css_valid(void) {
    char *base_dir = NULL;

#ifdef _WIN32
    char path[MAX_PATH];
    if (GetModuleFileNameA(NULL, path, MAX_PATH)) {
        base_dir = g_path_get_dirname(path);
    }
#else
    char *exe_path = g_file_read_link("/proc/self/exe", NULL);
    if (exe_path) {
        base_dir = g_path_get_dirname(exe_path);
        g_free(exe_path);
    }
#endif

    if (!base_dir) {
        base_dir = g_get_current_dir(); // fallback
    }

    char *assets_dir = g_build_filename(base_dir, "..", "assets", "css", NULL);
    char *css_filename = "dummy_test.css";
    char *css_path = g_build_filename(assets_dir, css_filename, NULL);

    /* Ensure the assets/css directory exists */
    g_mkdir_with_parents(assets_dir, 0700);

    /* Create a dummy CSS file */
    g_file_set_contents(css_path, "* { color: #f00; }", -1, NULL);

    GtkCssProvider* provider = load_css_from_file(css_filename);
    g_assert_nonnull(provider);

    /* Clean up the dummy file */
    remove(css_path);
    g_free(css_path);
    g_free(assets_dir);
    g_free(base_dir);
}

int main(int argc, char **argv) {
    g_test_init(&argc, &argv, NULL);
    g_test_add_func("/persistence/settings",          test_settings);
    g_test_add_func("/persistence/scores",            test_scores);
    g_test_add_func("/persistence/scores_lower_better", test_scores_lower_better);
    g_test_add_func("/persistence/load_missing_score",  test_load_missing_score);
    g_test_add_func("/persistence/css_path_traversal",  test_load_css_path_traversal);
    g_test_add_func("/persistence/theme_preservation",  test_theme_preservation);

    if (gtk_init_check()) {
        g_test_add_func("/persistence/css_valid",           test_load_css_valid);
    } else {
        g_message("Skipping /persistence/css_valid test because gtk_init_check failed");
    }

    return g_test_run();
}
