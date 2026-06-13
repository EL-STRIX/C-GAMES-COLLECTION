#ifndef PERSISTENCE_H
#define PERSISTENCE_H

// Save a player's score for a specific game
void save_score(const char *game_name, const char *player_name, int score, int is_lower_better);

// Load the top score for a specific game.
// Returns the score, and populates out_player_name (must be at least 50 chars).
// Returns -1 if no score is found.
int load_top_score(const char *game_name, char *out_player_name);

#endif
