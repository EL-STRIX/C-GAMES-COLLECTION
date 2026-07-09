#ifndef UI_UTILS_H
#define UI_UTILS_H

#include <gtk/gtk.h>
#include "constants.h"

// Creates a standardized white card container
GtkWidget* create_card_box(void);

// Standardized exit callback matching GTK4 Dialog API
void confirm_exit_response(GObject *source_object, GAsyncResult *res, gpointer user_data);

// Standard back button logic
void handle_header_back_clicked(GtkWidget *window, GtkWidget *stack, const char *game_page_name);

#endif // UI_UTILS_H
