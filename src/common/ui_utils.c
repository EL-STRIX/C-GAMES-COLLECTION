#include "ui_utils.h"
#include "persistence.h"
#include <stdio.h>
#ifdef _WIN32
#include <windows.h>
#endif
static GtkCssProvider *current_theme_provider = NULL;

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
    (void)user_data; /* window not needed — switch_to_launcher() uses the global */
    GtkAlertDialog *dialog = GTK_ALERT_DIALOG(source_object);
    GError *error = NULL;
    int response = gtk_alert_dialog_choose_finish(dialog, res, &error);
    if (error) {
        g_error_free(error);
        return;
    }

    /* Response 1 is the second button ("Return to Menu") */
    if (response == 1) {
        switch_to_launcher();
    }
}

void handle_header_back_clicked(GtkWidget *window, GtkWidget *stack, const char *game_page_name)
{
    const char *visible_child = gtk_stack_get_visible_child_name(GTK_STACK(stack));

    if (g_strcmp0(visible_child, game_page_name) == 0) {
        /* Player is mid-game: confirm before navigating away */
        GtkAlertDialog *dialog = gtk_alert_dialog_new("Are you sure you want to return to the main menu?");
        gtk_alert_dialog_set_detail(dialog, "Any unsaved progress will be lost.");
        const char *buttons[] = {"Cancel", "Return to Menu", NULL};
        gtk_alert_dialog_set_buttons(dialog, buttons);
        gtk_alert_dialog_set_cancel_button(dialog, 0);
        gtk_alert_dialog_set_default_button(dialog, 0);
        gtk_alert_dialog_choose(dialog, GTK_WINDOW(window), NULL, confirm_exit_response, NULL);
        g_object_unref(dialog);
    } else {
        /* Player is on welcome/result screen: go back silently */
        switch_to_launcher();
    }
}

void apply_global_theme(void) {
    char dummy_name[50];
    int theme_id;
    load_global_settings(dummy_name, sizeof(dummy_name), &theme_id);

    if (current_theme_provider != NULL) {
        gtk_style_context_remove_provider_for_display(gdk_display_get_default(), GTK_STYLE_PROVIDER(current_theme_provider));
        g_object_unref(current_theme_provider);
        current_theme_provider = NULL;
    }

    if (theme_id == 1 || theme_id == 2) {
        g_object_set(gtk_settings_get_default(), "gtk-application-prefer-dark-theme", TRUE, NULL);
    } else {
        g_object_set(gtk_settings_get_default(), "gtk-application-prefer-dark-theme", FALSE, NULL);
    }

    const char *css_filename = "theme_default.css"; // Default
    if (theme_id == 1) css_filename = "theme_dark.css";
    else if (theme_id == 2) css_filename = "theme_hacker.css";
    
    current_theme_provider = load_css_from_file(css_filename);
}

GtkWidget* create_game_header(const char *title, gboolean show_title_buttons)
{
    GtkWidget *header = gtk_header_bar_new();
    gtk_header_bar_set_show_title_buttons(GTK_HEADER_BAR(header), show_title_buttons);

    GtkWidget *title_lbl = gtk_label_new(title);
    gtk_widget_add_css_class(title_lbl, "header-title");
    gtk_header_bar_set_title_widget(GTK_HEADER_BAR(header), title_lbl);

    return header;
}

// Helper function to dynamically locate and load CSS from assets/css directory
GtkCssProvider* load_css_from_file(const char *filename) {
    if (strpbrk(filename, "/\\")) {
        g_warning("Security violation: path traversal detected in css filename '%s'", filename);
        return NULL;
    }
    
    char *base_dir = NULL;
    
#ifdef _WIN32
    char path[MAX_PATH];
    if (GetModuleFileNameA(NULL, path, MAX_PATH)) {
        base_dir = g_path_get_dirname(path);
    }
#else
    char *exe_path = g_file_read_link("/proc/self/exe", NULL);
    if (exe_path) {
        base_dir = g_path_get_dirname(exe_path);
        g_free(exe_path);
    }
#endif

    if (!base_dir) {
        base_dir = g_get_current_dir(); // fallback
    }
    
    // Construct path: base_dir/../assets/css/filename
    // Since executable is in 'bin', base_dir/../assets/css points to the correct location
    char *assets_dir = g_build_filename(base_dir, "..", "assets", "css", filename, NULL);
    
    GtkCssProvider *provider = gtk_css_provider_new();
    GFile *css_file = g_file_new_for_path(assets_dir);
    
    // Load it directly to screen
    gtk_css_provider_load_from_file(provider, css_file);
    gtk_style_context_add_provider_for_display(gdk_display_get_default(), 
                                               GTK_STYLE_PROVIDER(provider), 
                                               GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
                                               
    g_object_unref(css_file);
    g_free(assets_dir);
    g_free(base_dir);
    
    return provider;
}

GtkWidget* create_choice_button(const char *emoji, const char *label_text, GCallback callback, gpointer data) {
    GtkWidget *btn = gtk_button_new();
    gtk_widget_add_css_class(btn, "choice-btn");

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    gtk_widget_set_halign(box, GTK_ALIGN_CENTER);

    GtkWidget *lbl_emoji = gtk_label_new(emoji);
    gtk_widget_add_css_class(lbl_emoji, "choice-emoji");

    GtkWidget *lbl_text = gtk_label_new(label_text);
    gtk_widget_add_css_class(lbl_text, "choice-label");

    gtk_box_append(GTK_BOX(box), lbl_emoji);
    gtk_box_append(GTK_BOX(box), lbl_text);

    gtk_button_set_child(GTK_BUTTON(btn), box);
    g_signal_connect(btn, "clicked", callback, data);

    return btn;
}
