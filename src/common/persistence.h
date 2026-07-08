#ifndef PERSISTENCE_H
#define PERSISTENCE_H

#include <gtk/gtk.h>
#include <glib.h>

// Persistence functions
int load_top_score(const char *game_name, char *out_player_name, size_t out_size);
void save_score(const char *game_name, const char *player_name, int score, int is_lower_better);

// Settings functions
void save_global_settings(const char *player_name, int theme_id);
void load_global_settings(char *player_name, size_t out_size, int *theme_id);

// Utility functions
gboolean return_to_launcher(void);
void load_css_from_file(const char *filename);

#endif // PERSISTENCE_H
