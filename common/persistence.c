#include "persistence.h"
#include <stdio.h>
#include <string.h>

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
