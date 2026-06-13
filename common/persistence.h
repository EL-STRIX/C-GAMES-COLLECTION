#ifndef PERSISTENCE_H
#define PERSISTENCE_H

// Save a player's score for a specific game
void save_score(const char *game_id, const char *player_name, int score, int is_lower_better);
int load_top_score(const char *game_id, char *best_player);

void save_global_settings(const char *player_name, int theme_id);
void load_global_settings(char *player_name, int *theme_id);
void return_to_launcher(void);
void apply_theme(int theme_id);

#endif
