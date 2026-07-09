#ifndef GAMES_H
#define GAMES_H

#include <gtk/gtk.h>

// Game constructors that return the root GtkWidget (e.g. GtkBox or GtkOverlay)
// for the game, which can be appended to the launcher's GtkStack.

GtkWidget* ng_create_ui(void);
GtkWidget* rps_create_ui(void);
GtkWidget* sgw_create_ui(void);
GtkWidget* ttt_create_ui(void);

// Centralized stack manipulation (defined in launcher/main.c)
void switch_to_launcher(void);

#endif // GAMES_H
