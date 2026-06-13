#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>
#include <glib.h>
#ifdef _WIN32
#include <windows.h>
#endif

// --- PERSISTENCE & THEME ENGINE INJECTED ---
int load_top_score(const char *game_name, char *out_player_name) {
    char filename[100];
    snprintf(filename, sizeof(filename), "%s_score.ini", game_name);
    
    GKeyFile *kf = g_key_file_new();
    if (!g_key_file_load_from_file(kf, filename, G_KEY_FILE_NONE, NULL)) {
        if (out_player_name) strcpy(out_player_name, "None");
        g_key_file_free(kf);
        return -1;
    }
    
    int score = g_key_file_get_integer(kf, "Score", "Value", NULL);
    gchar *name = g_key_file_get_string(kf, "Score", "Player", NULL);
    if (name && out_player_name) {
        strncpy(out_player_name, name, 49);
        out_player_name[49] = '\0';
    } else if (out_player_name) {
        strcpy(out_player_name, "Unknown");
    }
    
    g_free(name);
    g_key_file_free(kf);
    return score;
}

void save_score(const char *game_name, const char *player_name, int score, int is_lower_better) {
    char top_player[50]; 
    int top_score = load_top_score(game_name, top_player);
    int is_new_record = (top_score == -1) || (is_lower_better ? (score < top_score) : (score > top_score));
    
    if (is_new_record) {
        char filename[100]; 
        snprintf(filename, sizeof(filename), "%s_score.ini", game_name);
        GKeyFile *kf = g_key_file_new();
        g_key_file_set_string(kf, "Score", "Player", player_name);
        g_key_file_set_integer(kf, "Score", "Value", score);
        g_key_file_save_to_file(kf, filename, NULL);
        g_key_file_free(kf);
    }
}

void save_global_settings(const char *player_name, int theme_id) {
    GKeyFile *kf = g_key_file_new();
    g_key_file_set_string(kf, "Settings", "PlayerName", player_name);
    g_key_file_set_integer(kf, "Settings", "ThemeID", theme_id);
    g_key_file_save_to_file(kf, "settings.ini", NULL);
    g_key_file_free(kf);
}

void load_global_settings(char *player_name, int *theme_id) {
    GKeyFile *kf = g_key_file_new();
    if (g_key_file_load_from_file(kf, "settings.ini", G_KEY_FILE_NONE, NULL)) {
        gchar *name = g_key_file_get_string(kf, "Settings", "PlayerName", NULL);
        if (name) {
            strncpy(player_name, name, 49);
            player_name[49] = '\0';
            g_free(name);
        } else {
            strcpy(player_name, "Player 1");
        }
        
        GError *err = NULL;
        int t = g_key_file_get_integer(kf, "Settings", "ThemeID", &err);
        if (err) {
            *theme_id = 0;
            g_error_free(err);
        } else {
            *theme_id = t;
        }
    } else {
        strcpy(player_name, "Player 1");
        *theme_id = 0;
    }
    g_key_file_free(kf);
}

gboolean return_to_launcher(void) {
    char *full_path = NULL; 
    const char *exe_name = "launcher.exe";
#ifdef _WIN32
    char path[MAX_PATH]; GetModuleFileNameA(NULL, path, MAX_PATH);
    char *dir = g_path_get_dirname(path); full_path = g_build_filename(dir, exe_name, NULL); g_free(dir);
#else
    char *exe_path = g_file_read_link("/proc/self/exe", NULL);
    if (exe_path) {
        char *dir = g_path_get_dirname(exe_path); full_path = g_build_filename(dir, exe_name, NULL);
        g_free(dir); g_free(exe_path);
    } else { 
        char *cwd = g_get_current_dir();
        full_path = g_build_filename(cwd, exe_name, NULL); 
        g_free(cwd);
    }
#endif
    
    GError *error = NULL;
    if (!g_spawn_command_line_async(full_path, &error)) {
        GtkAlertDialog *dialog = gtk_alert_dialog_new("Failed to return to launcher: %s\nPath: %s", error->message, full_path);
        gtk_alert_dialog_show(dialog, NULL);
        g_object_unref(dialog);
        g_error_free(error);
        g_free(full_path);
        return FALSE;
    }
    g_free(full_path);
    return TRUE;
}
// --- END INJECTED ENGINE ---

static void test_settings(void) {
    save_global_settings("Test\nPlayer", 2);
    char out_name[50];
    int out_theme = 0;
    load_global_settings(out_name, &out_theme);
    
    // GKeyFile handles newlines natively via escaping
    g_assert_cmpstr(out_name, ==, "Test\nPlayer");
    g_assert_cmpint(out_theme, ==, 2);
    
    remove("settings.ini");
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
    
    remove("test_game_score.ini");
}

int main(int argc, char **argv) {
    gtk_init();
    g_test_init(&argc, &argv, NULL);
    g_test_add_func("/persistence/settings", test_settings);
    g_test_add_func("/persistence/scores", test_scores);
    return g_test_run();
}
