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
static char* get_data_dir(void) {
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
    char *data_dir = g_build_filename(base_dir, "..", "data", NULL);
    g_free(base_dir);
    
    g_mkdir_with_parents(data_dir, 0700);
    return data_dir;
}

int load_top_score(const char *game_name, char *out_player_name, size_t out_size) {
    if (strpbrk(game_name, "/\\.")) {
        g_warning("Security violation: path traversal detected in game_name '%s'", game_name);
        return -1;
    }

    char *data_dir = get_data_dir();
    char *filename = g_strdup_printf("%s/%s_score.ini", data_dir, game_name);
    g_free(data_dir);
    
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
        return -1;
    }
    
    int score = g_key_file_get_integer(kf, "Score", "Value", NULL);
    gchar *name = g_key_file_get_string(kf, "Score", "Player", NULL);
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
        char *data_dir = get_data_dir();
        char *filename = g_strdup_printf("%s/%s_score.ini", data_dir, game_name);
        g_free(data_dir);
        GKeyFile *kf = g_key_file_new();
        g_key_file_set_string(kf, "Score", "Player", player_name);
        g_key_file_set_integer(kf, "Score", "Value", score);
        GError *error = NULL;
        if (!g_key_file_save_to_file(kf, filename, &error)) {
            g_warning("Failed to save score to %s: %s", filename, error ? error->message : "Unknown error");
            if (error) g_error_free(error);
        } else {
            g_message("Saved new top score to %s", filename);
        }
        g_key_file_free(kf);
        g_free(filename);
    }
}

void save_global_settings(const char *player_name, int theme_id) {

    GKeyFile *kf = g_key_file_new();
    g_key_file_set_string(kf, "Settings", "PlayerName", player_name);
    g_key_file_set_integer(kf, "Settings", "ThemeID", theme_id);
    GError *error = NULL;
    char *data_dir = get_data_dir();
    char *filename = g_strdup_printf("%s/settings.ini", data_dir);
    g_free(data_dir);
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
    char *data_dir = get_data_dir();
    char *filename = g_strdup_printf("%s/settings.ini", data_dir);
    g_free(data_dir);
    if (g_key_file_load_from_file(kf, filename, G_KEY_FILE_NONE, &error)) {
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
            if (theme_id) *theme_id = t;
        }
    } else {
        // Removed g_message here to prevent spamming the console on first launch
        if (error) g_error_free(error);
        if (out_size > 0) snprintf(player_name, out_size, "Player 1");
        if (theme_id) *theme_id = 0;
    }
    g_key_file_free(kf);
    g_free(filename);
}

gboolean return_to_launcher(void) {
    return TRUE;
}
