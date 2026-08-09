#include <glib.h>
#include <gtk/gtk.h>
#include <string.h>

static int mock_secret_number = 42;
#define get_secure_random_int_range mock_get_secure_random_int_range

int mock_get_secure_random_int_range(int begin, int end);
void switch_to_launcher(void);

int mock_get_secure_random_int_range(int begin, int end) {
    (void)begin;
    (void)end;
    return mock_secret_number;
}

void switch_to_launcher(void) {}

#include "../src/number_guessing/main.c"

static char *tmp_dir = NULL;

static void init_mock_app_data(NgAppData *app) {
    memset(app, 0, sizeof(NgAppData));

    app->stack = gtk_stack_new();

    GtkWidget *page_game = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *page_result = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_stack_add_named(GTK_STACK(app->stack), page_game, "page_game");
    gtk_stack_add_named(GTK_STACK(app->stack), page_result, "page_result");

    app->name_entry = gtk_entry_new();
    app->name_warning_label = gtk_label_new("");
    app->greeting_label = gtk_label_new("");
    app->guess_spin = gtk_spin_button_new_with_range(0, 150, 1);
    app->feedback_label = gtk_label_new("");
    app->feedback_revealer = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    app->attempts_label = gtk_label_new("");
    app->congrats_label = gtk_label_new("");
    app->secret_number_label = gtk_label_new("");
    app->total_attempts_label = gtk_label_new("");
    app->praise_label = gtk_label_new("");

    strcpy(app->player_name, "TestPlayer");
}

static void test_ng_start_game_logic(void) {
    NgAppData app;
    init_mock_app_data(&app);

    app.attempts = 5;
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(app.guess_spin), 99);
    gtk_widget_set_visible(app.feedback_revealer, TRUE);

    mock_secret_number = 42;
    ng_start_game_logic(&app);

    g_assert_cmpint(app.secret_number, ==, 42);
    g_assert_cmpint(app.attempts, ==, 0);
    g_assert_cmpfloat(gtk_spin_button_get_value(GTK_SPIN_BUTTON(app.guess_spin)), ==, 0.0);
    g_assert_cmpint(gtk_widget_get_visible(app.feedback_revealer), ==, FALSE);
}

static void test_ng_on_submit_guess_too_low(void) {
    NgAppData app;
    init_mock_app_data(&app);

    app.secret_number = 50;
    app.attempts = 0;

    gtk_spin_button_set_value(GTK_SPIN_BUTTON(app.guess_spin), 25);

    ng_on_submit_guess(NULL, &app);

    g_assert_cmpint(app.attempts, ==, 1);
    g_assert_cmpstr(gtk_label_get_text(GTK_LABEL(app.feedback_label)), ==, "That's too low! Aim a little higher!");
    g_assert_cmpint(gtk_widget_get_visible(app.feedback_revealer), ==, TRUE);
}

static void test_ng_on_submit_guess_too_high(void) {
    NgAppData app;
    init_mock_app_data(&app);

    app.secret_number = 50;
    app.attempts = 0;

    gtk_spin_button_set_value(GTK_SPIN_BUTTON(app.guess_spin), 75);

    ng_on_submit_guess(NULL, &app);

    g_assert_cmpint(app.attempts, ==, 1);
    g_assert_cmpstr(gtk_label_get_text(GTK_LABEL(app.feedback_label)), ==, "Overshot it! Try a smaller number.");
    g_assert_cmpint(gtk_widget_get_visible(app.feedback_revealer), ==, TRUE);
}

static void test_ng_on_submit_guess_out_of_bounds(void) {
    NgAppData app;
    init_mock_app_data(&app);

    app.secret_number = 50;
    app.attempts = 0;

    gtk_spin_button_set_value(GTK_SPIN_BUTTON(app.guess_spin), 150);

    ng_on_submit_guess(NULL, &app);

    g_assert_cmpint(app.attempts, ==, 0);
    g_assert_cmpint(gtk_widget_get_visible(app.feedback_revealer), ==, TRUE);
}

static void test_ng_on_submit_guess_correct(void) {
    NgAppData app;
    init_mock_app_data(&app);

    app.secret_number = 50;
    app.attempts = 3;

    gtk_spin_button_set_value(GTK_SPIN_BUTTON(app.guess_spin), 50);

    ng_on_submit_guess(NULL, &app);

    g_assert_cmpint(app.attempts, ==, 4);
    g_assert_cmpstr(gtk_stack_get_visible_child_name(GTK_STACK(app.stack)), ==, "page_result");
}

int main(int argc, char **argv) {
    gtk_init();
    g_test_init(&argc, &argv, NULL);

    GError *error = NULL;
    tmp_dir = g_dir_make_tmp("ng_test_XXXXXX", &error);
    if (error != NULL) {
        g_error("Failed to create tmp dir: %s", error->message);
    }

    char *data_dir = g_build_filename(tmp_dir, "data", NULL);
    g_mkdir_with_parents(data_dir, 0700);
    g_setenv("PERSISTENCE_TEST_DATA_DIR", tmp_dir, TRUE);
    g_free(data_dir);

    g_test_add_func("/number_guessing/start_game_logic", test_ng_start_game_logic);
    g_test_add_func("/number_guessing/guess_too_low", test_ng_on_submit_guess_too_low);
    g_test_add_func("/number_guessing/guess_too_high", test_ng_on_submit_guess_too_high);
    g_test_add_func("/number_guessing/guess_out_of_bounds", test_ng_on_submit_guess_out_of_bounds);
    g_test_add_func("/number_guessing/guess_correct", test_ng_on_submit_guess_correct);

    int ret = g_test_run();
    return ret;
}
