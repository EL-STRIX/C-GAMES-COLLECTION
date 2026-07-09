#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include "../common/persistence.h"
#include "../common/constants.h"
#include "../common/ui_utils.h"
#include "../common/games.h"

GtkWidget *main_window = NULL;
GtkWidget *global_app_stack = NULL;
GtkWidget *global_champ_list = NULL;

void refresh_hall_of_fame(void) {
    if (!global_champ_list) return;
    
    GtkWidget *child = gtk_widget_get_first_child(global_champ_list);
    while (child != NULL) {
        GtkWidget *next = gtk_widget_get_next_sibling(child);
        gtk_box_remove(GTK_BOX(global_champ_list), child);
        child = next;
    }

    struct { const char *id; const char *name; const char *fmt; } games[] = {
        {"number_guessing", "Number Guessing", "%s: %s (%d guesses)"},
        {"rps", "Rock Paper Scissors", "%s: %s (%d wins)"},
        {"sgw", "Snake Gun Water", "%s: %s (%d wins)"},
        {"ttt_gui", "Epic Tic Tac Toe", "%s: %s (%d wins)"}
    };
    
    int added = 0;
    for (int i = 0; i < 4; i++) {
        char player[50];
        int score = load_top_score(games[i].id, player, sizeof(player));
        if (score != -1) {
            char txt[100];
            snprintf(txt, sizeof(txt), games[i].fmt, games[i].name, player, score);
            GtkWidget *l = gtk_label_new(txt);
            gtk_widget_add_css_class(l, "champ-item");
            gtk_box_append(GTK_BOX(global_champ_list), l);
            added = 1;
        }
    }
    
    if (!added) {
        GtkWidget *l = gtk_label_new("No high scores yet. Play a game!");
        gtk_widget_add_css_class(l, "champ-item");
        gtk_box_append(GTK_BOX(global_champ_list), l);
    }
}


void switch_to_launcher(void) {
    if (global_app_stack) {
        refresh_hall_of_fame();
        gtk_stack_set_visible_child_name(GTK_STACK(global_app_stack), "launcher_home");
    }
}

static void on_save_settings(GtkButton *btn, gpointer user_data) {
    (void)btn;
    GtkWidget **widgets = (GtkWidget **)user_data;
    GtkEntryBuffer *buf = gtk_entry_get_buffer(GTK_ENTRY(widgets[0]));
    const char *name = gtk_entry_buffer_get_text(buf);
    int theme_id = (int)gtk_drop_down_get_selected(GTK_DROP_DOWN(widgets[1]));
    
    save_global_settings(name, theme_id);
    gtk_window_destroy(GTK_WINDOW(widgets[2]));
    g_free(widgets);
    
    // Automatically apply theme to launcher if changed
    apply_global_theme();
}

static void open_settings_dialog(GtkButton *btn, gpointer user_data) {
    (void)btn;
    GtkWindow *parent = GTK_WINDOW(user_data);
    GtkWidget *dialog = gtk_window_new();
    gtk_window_set_transient_for(GTK_WINDOW(dialog), parent);
    gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
    gtk_window_set_title(GTK_WINDOW(dialog), "Global Settings");
    gtk_window_set_default_size(GTK_WINDOW(dialog), 300, 200);
    
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_margin_start(box, 20); gtk_widget_set_margin_end(box, 20);
    gtk_widget_set_margin_top(box, 20); gtk_widget_set_margin_bottom(box, 20);
    
    char name[50];
    int theme_id;
    load_global_settings(name, sizeof(name), &theme_id);
    
    GtkWidget *lbl_name = gtk_label_new("Global Player Name:");
    GtkWidget *entry_name = gtk_entry_new();
    gtk_editable_set_text(GTK_EDITABLE(entry_name), name);
    
    GtkWidget *lbl_theme = gtk_label_new("Global Theme:");
    const char *themes[] = {"Default (Blue)", "Dark Mode", "Hacker", NULL};
    GtkWidget *dropdown = gtk_drop_down_new_from_strings(themes);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(dropdown), (guint)theme_id);
    
    GtkWidget *save_btn = gtk_button_new_with_label("Save Settings");
    gtk_widget_add_css_class(save_btn, "btn-primary");
    
    GtkWidget **widgets = g_new(GtkWidget*, 3);
    widgets[0] = entry_name;
    widgets[1] = dropdown;
    widgets[2] = dialog;
    g_signal_connect(save_btn, "clicked", G_CALLBACK(on_save_settings), widgets);
    
    gtk_box_append(GTK_BOX(box), lbl_name);
    gtk_box_append(GTK_BOX(box), entry_name);
    gtk_box_append(GTK_BOX(box), lbl_theme);
    gtk_box_append(GTK_BOX(box), dropdown);
    gtk_box_append(GTK_BOX(box), save_btn);
    
    gtk_window_set_child(GTK_WINDOW(dialog), box);
    gtk_window_present(GTK_WINDOW(dialog));
}

static void launch_game(GtkButton *btn, gpointer user_data)
{
    (void)btn;
    const char *game_id = (const char *)user_data;
    
    GtkStack *stack = GTK_STACK(global_app_stack);
    
    // Check if page already exists, if so destroy it to reset state cleanly
    GtkWidget *existing = gtk_stack_get_child_by_name(stack, game_id);
    if (existing) {
        gtk_stack_remove(stack, existing);
    }
    
    GtkWidget *game_page = NULL;
    if (strcmp(game_id, "number_guessing") == 0) {
        game_page = ng_create_ui();
    } else if (strcmp(game_id, "rock_paper_scissors") == 0) {
        game_page = rps_create_ui();
    } else if (strcmp(game_id, "snake_gun_water") == 0) {
        game_page = sgw_create_ui();
    } else if (strcmp(game_id, "tic_tac_toe") == 0) {
        game_page = ttt_create_ui();
    }
    
    if (game_page) {
        gtk_stack_add_named(stack, game_page, game_id);
        gtk_stack_set_visible_child_name(stack, game_id);
    }
}

GtkWidget* create_game_entry(const char *icon, const char *title, const char *desc, const char *game_id)
{
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_add_css_class(box, "card");
    
    GtkWidget *lbl_icon = gtk_label_new(icon);
    gtk_widget_set_margin_bottom(lbl_icon, 10);
    PangoAttrList *attrs = pango_attr_list_new();
    pango_attr_list_insert(attrs, pango_attr_scale_new(3.0));
    gtk_label_set_attributes(GTK_LABEL(lbl_icon), attrs);
    pango_attr_list_unref(attrs);
    
    GtkWidget *lbl_title = gtk_label_new(title);
    gtk_widget_add_css_class(lbl_title, "game-title");
    
    GtkWidget *lbl_desc = gtk_label_new(desc);
    gtk_widget_add_css_class(lbl_desc, "game-desc");
    gtk_label_set_wrap(GTK_LABEL(lbl_desc), TRUE);
    gtk_label_set_justify(GTK_LABEL(lbl_desc), GTK_JUSTIFY_CENTER);
    
    GtkWidget *btn_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    gtk_widget_set_halign(btn_box, GTK_ALIGN_CENTER);
    GtkWidget *play_icon = gtk_label_new("\u25b6");
    GtkWidget *play_lbl = gtk_label_new("Play Now");
    gtk_box_append(GTK_BOX(btn_box), play_icon);
    gtk_box_append(GTK_BOX(btn_box), play_lbl);
    
    GtkWidget *btn = gtk_button_new();
    gtk_button_set_child(GTK_BUTTON(btn), btn_box);
    gtk_widget_add_css_class(btn, "btn-primary");
    g_signal_connect(btn, "clicked", G_CALLBACK(launch_game), (gpointer)game_id);
    
    gtk_box_append(GTK_BOX(box), lbl_icon);
    gtk_box_append(GTK_BOX(box), lbl_title);
    gtk_box_append(GTK_BOX(box), lbl_desc);
    gtk_widget_set_vexpand(btn, FALSE);
    gtk_widget_set_valign(btn, GTK_ALIGN_END);
    gtk_widget_set_vexpand(lbl_desc, TRUE);
    gtk_widget_set_valign(lbl_desc, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(box), btn);
    
    return box;
}

static void activate(GtkApplication *app, gpointer user_data)
{
    (void)user_data;
    
    GtkWidget *window = gtk_application_window_new(app);
    main_window = window;
    gtk_window_set_title(GTK_WINDOW(window), "C Games Collection");
    gtk_window_set_default_size(GTK_WINDOW(window), 900, 700);
    gtk_window_maximize(GTK_WINDOW(window));
    
    apply_global_theme();
    
    global_app_stack = gtk_stack_new();
    gtk_stack_set_transition_type(GTK_STACK(global_app_stack), GTK_STACK_TRANSITION_TYPE_CROSSFADE);
    gtk_stack_set_transition_duration(GTK_STACK(global_app_stack), 300); // 300ms smooth crossfade
    
    // --- BUILD LAUNCHER HOME PAGE ---
    GtkWidget *main_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_halign(main_vbox, GTK_ALIGN_CENTER);
    
    GtkWidget *header_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    
    GtkWidget *title = gtk_label_new("C GAMES COLLECTION");
    gtk_widget_add_css_class(title, "header-title");
    gtk_widget_set_hexpand(title, TRUE);
    
    GtkWidget *btn_settings = gtk_button_new_from_icon_name("emblem-system-symbolic");
    gtk_widget_set_tooltip_text(btn_settings, "Global Settings");
    gtk_widget_add_css_class(btn_settings, "btn-secondary");
    gtk_widget_set_valign(btn_settings, GTK_ALIGN_CENTER);
    g_signal_connect(btn_settings, "clicked", G_CALLBACK(open_settings_dialog), window);
    
    gtk_box_append(GTK_BOX(header_box), title);
    gtk_box_append(GTK_BOX(header_box), btn_settings);
    
    GtkWidget *subtitle = gtk_label_new("Select a game from the library to begin your adventure.");
    gtk_widget_add_css_class(subtitle, "subtitle");
    
    gtk_box_append(GTK_BOX(main_vbox), header_box);
    gtk_box_append(GTK_BOX(main_vbox), subtitle);
    
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 20);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 20);
    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
    
    GtkWidget *g1 = create_game_entry("\U0001f522", "Number Guessing", "Read the computer's mind and guess the secret number.", "number_guessing");
    gtk_grid_attach(GTK_GRID(grid), g1, 0, 0, 1, 1);
    
    GtkWidget *g2 = create_game_entry("\u270a\u270b\u270c", "Rock Paper Scissors", "The classic battle of wits against an AI opponent.", "rock_paper_scissors");
    gtk_grid_attach(GTK_GRID(grid), g2, 1, 0, 1, 1);
    
    GtkWidget *g3 = create_game_entry("\U0001f40d\U0001f52b\U0001f4a7", "Snake Gun Water", "A fun variation of RPS with new rules and emojis.", "snake_gun_water");
    gtk_grid_attach(GTK_GRID(grid), g3, 0, 1, 1, 1);
    
    GtkWidget *g4 = create_game_entry("\u274c\u2b55", "Epic Tic Tac Toe", "An enhanced battle version of Tic Tac Toe.", "tic_tac_toe");
    gtk_grid_attach(GTK_GRID(grid), g4, 1, 1, 1, 1);
    
    GtkWidget *champ_frame = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_add_css_class(champ_frame, "card");
    
    GtkWidget *champ_label = gtk_label_new("HALL OF FAME");
    gtk_widget_add_css_class(champ_label, "champ-title");
    gtk_box_append(GTK_BOX(champ_frame), champ_label);
    
    global_champ_list = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_append(GTK_BOX(champ_frame), global_champ_list);
    refresh_hall_of_fame();
    
    gtk_box_append(GTK_BOX(main_vbox), grid);
    gtk_box_append(GTK_BOX(main_vbox), champ_frame);
    
    GtkWidget *scrolled = gtk_scrolled_window_new();
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scrolled), 550);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled), main_vbox);
    
    gtk_stack_add_named(GTK_STACK(global_app_stack), scrolled, "launcher_home");
    
    gtk_window_set_child(GTK_WINDOW(window), global_app_stack);
    gtk_window_present(GTK_WINDOW(window));
}

static GLogWriterOutput log_writer_func(GLogLevelFlags log_level, const GLogField *fields, gsize n_fields, gpointer user_data) {
    (void)user_data;
    for (gsize i = 0; i < n_fields; i++) {
        if (g_strcmp0(fields[i].key, "GLIB_DOMAIN") == 0) {
            const char *domain = (const char *)fields[i].value;
            if ((g_strcmp0(domain, "Pango") == 0 || g_strcmp0(domain, "Gtk") == 0) && (log_level == G_LOG_LEVEL_WARNING)) {
                return G_LOG_WRITER_HANDLED;
            }
        }
    }
    return g_log_writer_default(log_level, fields, n_fields, user_data);
}

int main(int argc, char **argv)
{
    g_log_set_writer_func(log_writer_func, NULL, NULL);
    
    GtkApplication *app = gtk_application_new("com.sujay.gamescollection", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}
