#include "persistence.h"
#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>
#ifdef _WIN32
#include <windows.h>
#endif
void save_score(const char *game_name, const char *player_name, int score, int is_lower_better) {
    // Read existing top score to see if we beat it
    char top_player[50];
    int top_score = load_top_score(game_name, top_player);
    
    int is_new_record = 0;
    if (top_score == -1) {
        is_new_record = 1;
    } else if (is_lower_better) {
        if (score < top_score) is_new_record = 1;
    } else {
        if (score > top_score) is_new_record = 1;
    }
    
    if (is_new_record) {
        char filename[100];
        snprintf(filename, sizeof(filename), "%s_score.dat", game_name);
        
        FILE *f = fopen(filename, "w");
        if (f) {
            fprintf(f, "%s\n%d\n", player_name, score);
            fclose(f);
        }
    }
}

int load_top_score(const char *game_name, char *out_player_name) {
    char filename[100];
    snprintf(filename, sizeof(filename), "%s_score.dat", game_name);
    
    FILE *f = fopen(filename, "r");
    if (!f) {
        if (out_player_name) strcpy(out_player_name, "None");
        return -1;
    }
    
    int score = -1;
    char name[50];
    if (fgets(name, sizeof(name), f)) {
        name[strcspn(name, "\n")] = 0; // Remove newline
        if (fscanf(f, "%d", &score) == 1) {
            if (out_player_name) strcpy(out_player_name, name);
        }
    }
    fclose(f);
    
    return score;
}

void save_global_settings(const char *player_name, int theme_id) {
    FILE *f = fopen("settings.dat", "w");
    if (f) {
        fprintf(f, "%s\n%d\n", player_name, theme_id);
        fclose(f);
    }
}

void load_global_settings(char *player_name, int *theme_id) {
    FILE *f = fopen("settings.dat", "r");
    if (f) {
        if (fgets(player_name, 50, f)) {
            player_name[strcspn(player_name, "\n")] = 0;
        } else {
            strcpy(player_name, "Player 1");
        }
        if (fscanf(f, "%d", theme_id) != 1) {
            *theme_id = 0;
        }
        fclose(f);
    } else {
        strcpy(player_name, "Player 1");
        *theme_id = 0;
    }
}

void return_to_launcher(void) {
    char *full_path = NULL;
    const char *exe_name = "launcher.exe";
#ifdef _WIN32
    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH);
    char *dir = g_path_get_dirname(path);
    full_path = g_build_filename(dir, exe_name, NULL);
    g_free(dir);
#else
    char *exe_path = g_file_read_link("/proc/self/exe", NULL);
    if (exe_path) {
        char *dir = g_path_get_dirname(exe_path);
        full_path = g_build_filename(dir, exe_name, NULL);
        g_free(dir);
        g_free(exe_path);
    } else {
        full_path = g_strdup_printf("./bin/%s", exe_name);
    }
#endif
    g_spawn_command_line_async(full_path, NULL);
    g_free(full_path);
}

void apply_theme(int theme_id) {
    const char *theme_css = "";
    if (theme_id == 1) { // Dark Mode
        theme_css = "window { background-color: #11111b; } .card { background-color: #1e1e2e; color: #cdd6f4; border: 1px solid #45475a; box-shadow: 0 0 10px rgba(255,255,255,0.1); } label { color: #cdd6f4; }";
    } else if (theme_id == 2) { // Hacker
        theme_css = "window { background-color: #0d0d0d; } .card { background-color: #000000; border: 2px solid #00ff00; box-shadow: 0 0 15px rgba(0,255,0,0.3); } label { color: #00ff00; font-family: monospace; } button { background-color: #002200; color: #00ff00; border: 1px solid #00ff00; }";
    }
    
    if (theme_id != 0) {
        GtkCssProvider *provider = gtk_css_provider_new();
        gtk_css_provider_load_from_data(provider, theme_css, -1);
        gtk_style_context_add_provider_for_display(gdk_display_get_default(), GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
        g_object_unref(provider);
    }
}
