/**
 * @file games.h
 * @brief Public interface for initializing individual game modules.
 *
 * Exposes the factory functions required by the central launcher to instantiate 
 * each game's UI and inject it into the global view stack.
 */

#ifndef GAMES_H
#define GAMES_H

#include <gtk/gtk.h>

/**
 * @brief Instantiate the Number Guessing game interface.
 * @return The root GtkWidget containing the game's isolated context.
 */
GtkWidget* ng_create_ui(void);

/**
 * @brief Instantiate the Rock Paper Scissors game interface.
 * @return The root GtkWidget containing the game's isolated context.
 */
GtkWidget* rps_create_ui(void);

/**
 * @brief Instantiate the Snake Gun Water game interface.
 * @return The root GtkWidget containing the game's isolated context.
 */
GtkWidget* sgw_create_ui(void);

/**
 * @brief Instantiate the Tic Tac Toe game interface.
 * @return The root GtkWidget containing the game's isolated context.
 */
GtkWidget* ttt_create_ui(void);

#endif /* GAMES_H */
