#include "ui_utils.h"
#include "persistence.h"

GtkWidget* create_card_box(void)
{
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
    gtk_widget_add_css_class(box, "card");
    gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(box, GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(box, CARD_WIDTH_REQUEST, -1);
    return box;
}

void confirm_exit_response(GObject *source_object, GAsyncResult *res, gpointer user_data) 
{
    GtkAlertDialog *dialog = GTK_ALERT_DIALOG(source_object);
    GError *error = NULL;
    int response = gtk_alert_dialog_choose_finish(dialog, res, &error);
    if (error) {
        g_error_free(error);
        return;
    }
    
    // Response 1 is the second button ("Return to Menu")
    if (response == 1) {
        if (return_to_launcher()) {
            GtkWindow *window = GTK_WINDOW(user_data);
            GtkApplication *gtk_app = gtk_window_get_application(window);
            g_application_quit(G_APPLICATION(gtk_app));
        }
    }
}

void handle_header_back_clicked(GtkWidget *window, GtkWidget *stack, const char *game_page_name)
{
    const char *visible_child = gtk_stack_get_visible_child_name(GTK_STACK(stack));
    
    if (g_strcmp0(visible_child, game_page_name) == 0) {
        GtkAlertDialog *dialog = gtk_alert_dialog_new("Are you sure you want to return to the main menu?");
        gtk_alert_dialog_set_detail(dialog, "Any unsaved progress will be lost.");
        const char *buttons[] = {"Cancel", "Return to Menu", NULL};
        gtk_alert_dialog_set_buttons(dialog, buttons);
        gtk_alert_dialog_set_cancel_button(dialog, 0);
        gtk_alert_dialog_set_default_button(dialog, 0);
        gtk_alert_dialog_choose(dialog, GTK_WINDOW(window), NULL, confirm_exit_response, window);
        g_object_unref(dialog);
    } else {
        if (return_to_launcher()) {
            GtkApplication *gtk_app = gtk_window_get_application(GTK_WINDOW(window));
            g_application_quit(G_APPLICATION(gtk_app));
        }
    }
}
