/**
 * @file ui_utils.h
 * @brief Shared UI utility functions used across all games and the launcher.
 */

#ifndef UI_UTILS_H
#define UI_UTILS_H

#include <gtk/gtk.h>
#include "constants.h"

/**
 * @brief Create a standardized white card container (vertically oriented GtkBox).
 * @return A new GtkWidget styled with the "card" CSS class.
 */
GtkWidget* create_card_box(void);

/**
 * @brief Async completion callback for the "Return to Menu?" alert dialog.
 *        Connected internally by handle_header_back_clicked().
 */
void confirm_exit_response(GObject *source_object, GAsyncResult *res, gpointer user_data);

/**
 * @brief Handle the back/menu button click with context-aware behaviour.
 *        If the player is mid-game (on game_page_name), shows a confirmation dialog.
 *        If on the welcome or result screen, navigates back silently.
 * @param window         The game's root GtkWidget (used as dialog parent).
 * @param stack          The game's GtkStack.
 * @param game_page_name The stack child name that represents the active game board.
 */
void handle_header_back_clicked(GtkWidget *window, GtkWidget *stack, const char *game_page_name);

/**
 * @brief Read the saved theme ID and apply the corresponding CSS provider globally.
 *        Safe to call multiple times — replaces the previous provider cleanly.
 */
void apply_global_theme(void);

/* Defined in launcher/main.c; declared here so ui_utils.c can call it. */
void switch_to_launcher(void);

#endif /* UI_UTILS_H */
