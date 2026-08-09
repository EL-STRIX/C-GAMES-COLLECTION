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
    
    char *base_dir = get_base_dir();
    
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

void add_developer_footer(GtkWidget *box)
{
    GtkWidget *footer = gtk_label_new("Developed by SUJAY PAUL");
    gtk_widget_add_css_class(footer, "footer-credit");
    gtk_box_append(GTK_BOX(box), footer);
}
