/**
 * @file persistence.h
 * @brief Public API for the persistence engine (scores, settings) and CSS loader.
 *
 * This module handles all file I/O for player data and application settings
 * using GLib's GKeyFile backend for reliable cross-platform INI storage.
 */

#ifndef PERSISTENCE_H
#define PERSISTENCE_H

#include <stddef.h>  /* size_t */
#include <gtk/gtk.h> /* GtkCssProvider, gboolean — kept for CSS loader return type */
#include <glib.h>

/**
 * @brief Load the top score for a given game.
 * @param game_name   Short game identifier (e.g. "rps"), no path separators.
 * @param out_player_name  Buffer to receive the champion's name.
 * @param out_size    Size of out_player_name buffer.
 * @return The top score, or -1 if no score file exists.
 */
int load_top_score(const char *game_name, char *out_player_name, size_t out_size);

/**
 * @brief Save a new score for a game (only saves if it beats the current record).
 * @param game_name       Short game identifier.
 * @param player_name     Name of the scoring player.
 * @param score           The score value.
 * @param is_lower_better Non-zero if a lower score is better (e.g. fewer guesses).
 */
void save_score(const char *game_name, const char *player_name, int score, int is_lower_better);

/**
 * @brief Persist the global player name and theme selection.
 * @param player_name  Player name string. Must not be NULL.
 * @param theme_id     Theme index (0=Default, 1=Dark, 2=Hacker).
 *                     Pass -1 to preserve the existing theme unchanged.
 */
void save_global_settings(const char *player_name, int theme_id);

/**
 * @brief Load the global player name and theme from settings.ini.
 *        Falls back to recovering the player name from game history if settings.ini
 *        is missing, and defaults to "Player 1" if no history exists.
 * @param player_name  Output buffer for player name.
 * @param out_size     Size of the player_name buffer.
 * @param theme_id     Output pointer for theme ID. May be NULL.
 */
void load_global_settings(char *player_name, size_t out_size, int *theme_id);

/**
 * @brief Signals the launcher to become the active view (no-op stub for future use).
 * @return Always TRUE.
 */
gboolean return_to_launcher(void);

/**
 * @brief Load a CSS file from the assets/css directory and apply it to the display.
 * @param filename  Basename only (e.g. "theme_dark.css"). Path separators are rejected.
 * @return A new GtkCssProvider already applied, or NULL on security violation.
 *         Caller does NOT need to unref — provider is owned by the style context.
 */
GtkCssProvider* load_css_from_file(const char *filename);

#endif /* PERSISTENCE_H */
