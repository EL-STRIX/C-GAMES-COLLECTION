#include "persistence.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#endif

// Helper function to dynamically locate and load CSS from assets/css directory
GtkCssProvider* load_css_from_file(const char *filename) {
    if (strpbrk(filename, "/\\")) {
        g_warning("Security violation: path traversal detected in css filename '%s'", filename);
        return NULL;
    }
    
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
    
    // Construct path: base_dir/../assets/css/filename
    // Since executable is in 'bin', base_dir/../assets/css points to the correct location
    char *assets_dir = g_build_filename(base_dir, "..", "assets", "css", filename, NULL);
    
    GtkCssProvider *provider = gtk_css_provider_new();
    GFile *css_file = g_file_new_for_path(assets_dir);
    
    // Load it directly to screen
    gtk_css_provider_load_from_file(provider, css_file);
    gtk_style_context_add_provider_for_display(gdk_display_get_default(), 
                                               GTK_STYLE_PROVIDER(provider), 
                                               GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
                                               
    g_object_unref(css_file);
    g_free(assets_dir);
    g_free(base_dir);
    
    return provider;
}


// Helper function to dynamically locate the data directory relative to the executable
static const char* get_data_dir(void) {
    static char *cached_data_dir = NULL;
    if (cached_data_dir) {
        return cached_data_dir;
    }

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
    
    // Construct path: base_dir/../data
    cached_data_dir = g_build_filename(base_dir, "..", "data", NULL);
    g_free(base_dir);
    
    g_mkdir_with_parents(cached_data_dir, 0700);
    return cached_data_dir;
}

typedef struct {
    char game_name[32];
    char player_name[50];
    int score;
    gboolean is_cached;
} CachedScore;

#define MAX_CACHED_SCORES 10
static CachedScore score_cache[MAX_CACHED_SCORES];

static void update_score_cache(const char *game_name, const char *player_name, int score) {
    int empty_slot = -1;
    for (int i = 0; i < MAX_CACHED_SCORES; i++) {
        if (score_cache[i].is_cached && strcmp(score_cache[i].game_name, game_name) == 0) {
            strncpy(score_cache[i].player_name, player_name ? player_name : "Unknown", sizeof(score_cache[i].player_name) - 1);
            score_cache[i].player_name[sizeof(score_cache[i].player_name) - 1] = '\0';
            score_cache[i].score = score;
            return;
        }
        if (!score_cache[i].is_cached && empty_slot == -1) {
            empty_slot = i;
        }
    }
    if (empty_slot != -1) {
        strncpy(score_cache[empty_slot].game_name, game_name, sizeof(score_cache[empty_slot].game_name) - 1);
        score_cache[empty_slot].game_name[sizeof(score_cache[empty_slot].game_name) - 1] = '\0';
        strncpy(score_cache[empty_slot].player_name, player_name ? player_name : "Unknown", sizeof(score_cache[empty_slot].player_name) - 1);
        score_cache[empty_slot].player_name[sizeof(score_cache[empty_slot].player_name) - 1] = '\0';
        score_cache[empty_slot].score = score;
        score_cache[empty_slot].is_cached = TRUE;
    }
}

int load_top_score(const char *game_name, char *out_player_name, size_t out_size) {
    if (strpbrk(game_name, "/\\.")) {
        g_warning("Security violation: path traversal detected in game_name '%s'", game_name);
        return -1;
    }

    for (int i = 0; i < MAX_CACHED_SCORES; i++) {
        if (score_cache[i].is_cached && strcmp(score_cache[i].game_name, game_name) == 0) {
            if (out_player_name && out_size > 0) {
                if (score_cache[i].score == -1) {
                    snprintf(out_player_name, out_size, "None");
                } else {
                    strncpy(out_player_name, score_cache[i].player_name, out_size - 1);
                    out_player_name[out_size - 1] = '\0';
                }
            }
            return score_cache[i].score;
        }
    }

    const char *data_dir = get_data_dir();
    char *filename = g_strdup_printf("%s/%s_score.ini", data_dir, game_name);
    
    GKeyFile *kf = g_key_file_new();
    GError *error = NULL;
    if (!g_key_file_load_from_file(kf, filename, G_KEY_FILE_NONE, &error)) {
        // Log cleanly to console if not found, this is normal for first runs
        // We removed g_message here to prevent spamming the console on first launch
        if (error) g_error_free(error);
        if (out_player_name && out_size > 0) {
            snprintf(out_player_name, out_size, "None");
        }
        g_key_file_free(kf);
        g_free(filename);
        update_score_cache(game_name, "None", -1);
        return -1;
    }
    
    int score = g_key_file_get_integer(kf, "Score", "Value", NULL);
    gchar *name = g_key_file_get_string(kf, "Score", "Player", NULL);

    update_score_cache(game_name, name ? name : "Unknown", score);

    if (name && out_player_name && out_size > 0) {
        strncpy(out_player_name, name, out_size - 1);
        out_player_name[out_size - 1] = '\0';
    } else if (out_player_name && out_size > 0) {
        snprintf(out_player_name, out_size, "Unknown");
    }
    
    g_free(name);
    g_key_file_free(kf);
    g_free(filename);
    return score;
}

void save_score(const char *game_name, const char *player_name, int score, int is_lower_better) {
    if (strpbrk(game_name, "/\\.")) {
        g_warning("Security violation: path traversal detected in game_name '%s'", game_name);
        return;
    }

    char top_player[50]; 
    int top_score = load_top_score(game_name, top_player, sizeof(top_player));
    int is_new_record = (top_score == -1) || (is_lower_better ? (score < top_score) : (score > top_score));
    
    if (is_new_record) {
        const char *data_dir = get_data_dir();
        char *filename = g_strdup_printf("%s/%s_score.ini", data_dir, game_name);
        GKeyFile *kf = g_key_file_new();
        g_key_file_set_string(kf, "Score", "Player", player_name);
        g_key_file_set_integer(kf, "Score", "Value", score);
        GError *error = NULL;
        if (!g_key_file_save_to_file(kf, filename, &error)) {
            g_warning("Failed to save score to %s: %s", filename, error ? error->message : "Unknown error");
            if (error) g_error_free(error);
        } else {
            g_message("Saved new top score to %s", filename);
            update_score_cache(game_name, player_name, score);
        }
        g_key_file_free(kf);
        g_free(filename);
    }
}

void save_global_settings(const char *player_name, int theme_id) {

    /* If theme_id == -1, preserve the existing theme from the saved file */
    if (theme_id == -1) {
        char dummy_name[50];
        int existing_theme = 0;
        load_global_settings(dummy_name, sizeof(dummy_name), &existing_theme);
        theme_id = existing_theme;
    }

    GKeyFile *kf = g_key_file_new();
    g_key_file_set_string(kf, "Settings", "PlayerName", player_name);
    g_key_file_set_integer(kf, "Settings", "ThemeID", theme_id);
    GError *error = NULL;
    const char *data_dir = get_data_dir();
    char *filename = g_strdup_printf("%s/settings.ini", data_dir);
    if (!g_key_file_save_to_file(kf, filename, &error)) {
        g_warning("Failed to save global settings: %s", error ? error->message : "Unknown error");
        if (error) g_error_free(error);
    } else {
        g_message("Saved global settings successfully.");
    }
    g_key_file_free(kf);
    g_free(filename);
}

void load_global_settings(char *player_name, size_t out_size, int *theme_id) {
    GKeyFile *kf = g_key_file_new();
    GError *error = NULL;
    const char *data_dir = get_data_dir();
    char *filename = g_strdup_printf("%s/settings.ini", data_dir);
    if (g_key_file_load_from_file(kf, filename, G_KEY_FILE_NONE, &error)) {
        /* settings.ini found — read name and theme directly */
        gchar *name = g_key_file_get_string(kf, "Settings", "PlayerName", NULL);
        if (name && out_size > 0) {
            strncpy(player_name, name, out_size - 1);
            player_name[out_size - 1] = '\0';
            g_free(name);
        } else if (out_size > 0) {
            snprintf(player_name, out_size, "Player 1");
        }

        GError *err = NULL;
        int t = g_key_file_get_integer(kf, "Settings", "ThemeID", &err);
        if (err) {
            if (theme_id) *theme_id = 0;
            g_error_free(err);
        } else {
            if (theme_id) *theme_id = (t >= 0) ? t : 0;
        }
    } else {
        /* settings.ini missing — try to recover player name from game history */
        if (error) g_error_free(error);

        if (out_size > 0) {
            char temp_player[50];
            if (load_top_score("rps", temp_player, sizeof(temp_player)) != -1
                    && strcmp(temp_player, "Unknown") != 0
                    && strcmp(temp_player, "None") != 0
                    && strcmp(temp_player, "Guest") != 0) {
                snprintf(player_name, out_size, "%s", temp_player);
            } else if (load_top_score("number_guessing", temp_player, sizeof(temp_player)) != -1
                    && strcmp(temp_player, "Unknown") != 0
                    && strcmp(temp_player, "None") != 0
                    && strcmp(temp_player, "Guest") != 0) {
                snprintf(player_name, out_size, "%s", temp_player);
            } else if (load_top_score("sgw", temp_player, sizeof(temp_player)) != -1
                    && strcmp(temp_player, "Unknown") != 0
                    && strcmp(temp_player, "None") != 0
                    && strcmp(temp_player, "Guest") != 0) {
                snprintf(player_name, out_size, "%s", temp_player);
            } else if (load_top_score("ttt_gui", temp_player, sizeof(temp_player)) != -1
                    && strcmp(temp_player, "Unknown") != 0
                    && strcmp(temp_player, "None") != 0
                    && strcmp(temp_player, "Guest") != 0) {
                snprintf(player_name, out_size, "%s", temp_player);
            } else {
                snprintf(player_name, out_size, "Player 1");
            }
        }
        if (theme_id) *theme_id = 0;
    }
    g_key_file_free(kf);
    g_free(filename);
}

