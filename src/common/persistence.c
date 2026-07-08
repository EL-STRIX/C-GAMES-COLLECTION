#include "persistence.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#endif

// Helper function to dynamically locate and load CSS from assets/css directory
void load_css_from_file(const char *filename) {
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
    g_object_unref(provider);
    g_free(assets_dir);
    g_free(base_dir);
}

int load_top_score(const char *game_name, char *out_player_name) {
    // Ensure data directory exists relative to executable if possible, 
    // or just assume current working directory is project root where bin and data reside.
    g_mkdir_with_parents("data", 0755);
    char filename[100];
    snprintf(filename, sizeof(filename), "data/%s_score.ini", game_name);
    
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
    g_mkdir_with_parents("data", 0755);
    char top_player[50]; 
    int top_score = load_top_score(game_name, top_player);
    int is_new_record = (top_score == -1) || (is_lower_better ? (score < top_score) : (score > top_score));
    
    if (is_new_record) {
        char filename[100]; 
        snprintf(filename, sizeof(filename), "data/%s_score.ini", game_name);
        GKeyFile *kf = g_key_file_new();
        g_key_file_set_string(kf, "Score", "Player", player_name);
        g_key_file_set_integer(kf, "Score", "Value", score);
        g_key_file_save_to_file(kf, filename, NULL);
        g_key_file_free(kf);
    }
}

void save_global_settings(const char *player_name, int theme_id) {
    g_mkdir_with_parents("data", 0755);
    GKeyFile *kf = g_key_file_new();
    g_key_file_set_string(kf, "Settings", "PlayerName", player_name);
    g_key_file_set_integer(kf, "Settings", "ThemeID", theme_id);
    g_key_file_save_to_file(kf, "data/settings.ini", NULL);
    g_key_file_free(kf);
}

void load_global_settings(char *player_name, int *theme_id) {
    GKeyFile *kf = g_key_file_new();
    if (g_key_file_load_from_file(kf, "data/settings.ini", G_KEY_FILE_NONE, NULL)) {
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
    return TRUE;
}
