#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>
#include <glib.h>
#ifdef _WIN32
#include <windows.h>
#endif

#include "../src/common/persistence.h"

static void test_settings(void) {
    save_global_settings("Test\nPlayer", 2);
    char out_name[50];
    int out_theme = 0;
    load_global_settings(out_name, &out_theme);
    
    // GKeyFile handles newlines natively via escaping
    g_assert_cmpstr(out_name, ==, "Test\nPlayer");
    g_assert_cmpint(out_theme, ==, 2);
    
    remove("data/settings.ini");
}

static void test_scores(void) {
    // is_lower_better = 0 (higher is better)
    save_score("test_game", "Alice", 100, 0);
    
    char top_player[50];
    int top_score = load_top_score("test_game", top_player);
    g_assert_cmpstr(top_player, ==, "Alice");
    g_assert_cmpint(top_score, ==, 100);
    
    // Bob scores lower, should not save
    save_score("test_game", "Bob", 50, 0);
    top_score = load_top_score("test_game", top_player);
    g_assert_cmpstr(top_player, ==, "Alice");
    g_assert_cmpint(top_score, ==, 100);
    
    // Charlie scores higher, should save
    save_score("test_game", "Charlie", 200, 0);
    top_score = load_top_score("test_game", top_player);
    g_assert_cmpstr(top_player, ==, "Charlie");
    g_assert_cmpint(top_score, ==, 200);
    
    remove("data/test_game_score.ini");
}

int main(int argc, char **argv) {
    gtk_init();
    g_test_init(&argc, &argv, NULL);
    g_test_add_func("/persistence/settings", test_settings);
    g_test_add_func("/persistence/scores", test_scores);
    return g_test_run();
}
