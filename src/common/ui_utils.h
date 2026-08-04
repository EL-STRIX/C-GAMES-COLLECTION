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

/**
 * @brief Create a standardized header bar for a game screen.
 * @param title The text to display in the header bar.
 * @param show_title_buttons Whether to show the standard window title buttons.
 * @return A new GtkWidget (GtkHeaderBar).
 */
GtkWidget* create_game_header(const char *title, gboolean show_title_buttons);

/**
 * @brief Load a CSS file from the assets/css directory and apply it to the display.
 * @param filename  Basename only (e.g. "theme_dark.css"). Path separators are rejected.
 * @return A new GtkCssProvider already applied, or NULL on security violation.
 *         Caller does NOT need to unref — provider is owned by the style context.
 */
GtkCssProvider* load_css_from_file(const char *filename);

/**
 * @brief Create a choice button with an emoji and label (used in RPS and SGW).
 * @param emoji      The emoji string to display.
 * @param label_text The label text to display below the emoji.
 * @param callback   The callback to connect to the "clicked" signal.
 * @param data       User data to pass to the callback.
 * @return A new GtkWidget containing the button.
 */
GtkWidget* create_choice_button(const char *emoji, const char *label_text, GCallback callback, gpointer data);

#endif /* UI_UTILS_H */
