/*
 * ----------------------------------------------------------------------------
 * Project: Number Guessing Game (GTK4)
 * Developer: Sujay Paul
 * Date: 24-11-2025
 * Email: sujaypaul892@gmail.com
 * ----------------------------------------------------------------------------
 */

#include <gtk/gtk.h> // Main library for the Graphical User Interface
#include <stdlib.h>  // Used for random numbers (rand)
#include <time.h>    // Used to set the random seed based on time
#include <string.h>  // Used for text/string handling
// --- PERSISTENCE & THEME ENGINE INJECTED ---
#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>
#ifdef _WIN32
#include <windows.h>
#endif

int load_top_score(const char *game_name, char *out_player_name) {
    g_mkdir_with_parents("data", 0755);
    char filename[100];
    snprintf(filename, sizeof(filename), "data/%s_score.ini", game_name);
    
    GKeyFile *kf = g_key_file_new();
    if (!g_key_file_load_from_file(kf, filename, G_KEY_FILE_NONE, NULL)) {
        if (out_player_name) strcpy(out_player_name, "None");
        g_key_file_free(kf);
        return -1;
    }
    
    int score = g_key_file_get_integer(kf, "Score", "Value", NULL);
    gchar *name = g_key_file_get_string(kf, "Score", "Player", NULL);
    if (name && out_player_name) {
        strncpy(out_player_name, name, 49);
        out_player_name[49] = '\0';
    } else if (out_player_name) {
        strcpy(out_player_name, "Unknown");
    }
    
    g_free(name);
    g_key_file_free(kf);
    return score;
}

void save_score(const char *game_name, const char *player_name, int score, int is_lower_better) {
    char top_player[50]; 
    int top_score = load_top_score(game_name, top_player);
    int is_new_record = (top_score == -1) || (is_lower_better ? (score < top_score) : (score > top_score));
    
    if (is_new_record) {
        g_mkdir_with_parents("data", 0755);
        char filename[100]; 
        snprintf(filename, sizeof(filename), "data/%s_score.ini", game_name);
        GKeyFile *kf = g_key_file_new();
        g_key_file_set_string(kf, "Score", "Player", player_name);
        g_key_file_set_integer(kf, "Score", "Value", score);
        g_key_file_save_to_file(kf, filename, NULL);
        g_key_file_free(kf);
    }
}

void save_global_settings(const char *player_name, int theme_id) {
    GKeyFile *kf = g_key_file_new();
    g_mkdir_with_parents("data", 0755);
    g_key_file_set_string(kf, "Settings", "PlayerName", player_name);
    g_key_file_set_integer(kf, "Settings", "ThemeID", theme_id);
    g_key_file_save_to_file(kf, "data/settings.ini", NULL);
    g_key_file_free(kf);
}

void load_global_settings(char *player_name, int *theme_id) {
    g_mkdir_with_parents("data", 0755);
    GKeyFile *kf = g_key_file_new();
    if (g_key_file_load_from_file(kf, "data/settings.ini", G_KEY_FILE_NONE, NULL)) {
        gchar *name = g_key_file_get_string(kf, "Settings", "PlayerName", NULL);
        if (name) {
            strncpy(player_name, name, 49);
            player_name[49] = '\0';
            g_free(name);
        } else {
            strcpy(player_name, "Player 1");
        }
        
        GError *err = NULL;
        int t = g_key_file_get_integer(kf, "Settings", "ThemeID", &err);
        if (err) {
            *theme_id = 0;
            g_error_free(err);
        } else {
            *theme_id = t;
        }
    } else {
        strcpy(player_name, "Player 1");
        *theme_id = 0;
    }
    g_key_file_free(kf);
}

gboolean return_to_launcher(void) {
    return TRUE;
}

static GtkCssProvider *current_theme_provider = NULL;

void apply_theme(int theme_id) {
    // Disabled as per user request to keep game pages strictly white/blue regardless of launcher theme.
    // The launcher can be dark/hacker, but the game screens stay professional white/blue.
}
// --- END INJECTED ENGINE ---

const char *css_data =
    "* { font-family: \"Segoe UI Emoji\", \"Noto Color Emoji\", sans-serif; }"
    "headerbar { min-height: 60px; background-color: #1e1e2e; color: #cdd6f4; border-bottom: 1px solid #11111b; }"
    ".header-title { font-size: 20pt; font-weight: 900; color: #ffffff; letter-spacing: 1px; }"
    "windowcontrols button { min-width: 40px; min-height: 40px; border-radius: 5px; }"
    "window { background-color: #f0f4f8; }"
    "label { color: #0f172a; }"
    "button { all: unset; border-radius: 12px; padding: 12px 24px; transition: all 0.2s ease; }"
    ".card { background-color: #ffffff; border-radius: 20px; padding: 40px; box-shadow: 0 10px 40px rgba(0,0,0,0.1); }"
    ".game-title { font-size: 24pt; font-weight: 900; color: #2563eb; margin-bottom: 10px; }"
    ".name-question { font-size: 14pt; font-weight: 500; color: #475569; margin-bottom: 20px; }"
    ".welcome-text { font-size: 18pt; font-weight: 800; color: #2563eb; margin-bottom: 20px; }"
    ".input-field { font-size: 14pt; padding: 12px; border: 2px solid #cbd5e1; border-radius: 10px; }"
    ".input-field:focus { border-color: #2563eb; }"
    ".guess-cheer-text { font-size: 14pt; font-weight: 600; color: #475569; }"
    ".btn-blue { background-color: #2563eb; color: #ffffff; font-weight: bold; font-size: 14pt; border-radius: 12px; }"
    ".btn-blue:hover { background-color: #3b82f6; box-shadow: 0 4px 12px rgba(37,99,235,0.3); }"
    ".btn-blue:active { background-color: #1d4ed8; }"
    ".btn-exit { background-color: #ef4444; color: #ffffff; font-weight: bold; font-size: 12pt; border-radius: 10px; padding: 10px 20px; }"
    ".btn-exit:hover { background-color: #f87171; box-shadow: 0 4px 12px rgba(239,68,68,0.3); }"
    ".feedback-box { background-color: #fee2e2; color: #b91c1c; border-radius: 12px; padding: 15px; margin-top: 20px; font-weight: bold; font-size: 14pt; }"
    ".success-text { font-size: 16pt; font-weight: bold; color: #0f172a; }"
    ".big-number { font-size: 40pt; font-weight: 900; color: #22c55e; margin: 20px; text-shadow: 0 4px 12px rgba(34,197,94,0.2); }"
    ".tip-text { color: #64748b; font-size: 12pt; margin-top: 20px; }"
    ".warning-text { color: #ef4444; font-weight: 700; font-size: 12pt; margin-top: 10px; }"
    ".attempts-text { color: #2563eb; font-size: 16pt; font-weight: 800; }"
    ".result-small-text { font-size: 16pt; font-weight: 600; color: #475569; }"
    ".result-performance-text { font-size: 18pt; font-weight: 800; color: #3b82f6; margin-top: 15px; }"
    ".dev-footer { font-size: 10pt; color: #94a3b8; font-weight: 600; margin-top: 20px; }";

// --- GAME DATA ---
// This structure holds all the information our game needs to run.
typedef struct
{
    GtkWidget *stack;  // Manages switching between screens (Welcome -> Game -> Result)
    GtkWidget *window; // The main application window

    // -- Widgets (User Interface elements) --
    GtkWidget *name_entry;         // Text box to type name
    GtkWidget *name_warning_label; // Error text if name is empty

    GtkWidget *greeting_label;    // "Welcome, [Name]!"
    GtkWidget *guess_spin;        // Number spinner input
    GtkWidget *feedback_label;    // Says "Too High" or "Too Low"
    GtkWidget *feedback_revealer; // Shows/Hides the feedback message
    GtkWidget *attempts_label;    // "Attempts: 5"

    GtkWidget *congrats_label;       // "CONGRATULATIONS!"
    GtkWidget *secret_number_label;  // Shows the answer at the end
    GtkWidget *total_attempts_label; // Final attempt count
    GtkWidget *praise_label;         // "Mastermind!", "Good Job!", etc.

    // -- Logic Variables --
    int secret_number;     // The random number the computer picks
    int attempts;          // Counts how many guesses the player made
    char player_name[100]; // Saves the player's name

} GameApp;

// --- GAME LOGIC FUNCTIONS ---

// 1. Resets the game to start a new round
static void start_game_logic(GameApp *app)
{
    srand(time(0));                          // Randomize based on current time
    app->secret_number = (rand() % 100) + 1; // Pick number 1-100
    app->attempts = 0;

    // Reset UI controls
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(app->guess_spin), 0);
    gtk_label_set_text(GTK_LABEL(app->feedback_label), "");
    gtk_widget_set_visible(app->feedback_revealer, FALSE); // Hide feedback box
    gtk_label_set_text(GTK_LABEL(app->attempts_label), "Attempts: 0");

    // Create the greeting message
    char greet[200];
    snprintf(greet, sizeof(greet),
             "Welcome, %s!\nI'm thinking of a secret number (1–100).\nCan you read my mind?",
             app->player_name);

    gtk_label_set_text(GTK_LABEL(app->greeting_label), greet);

    // Show the Game Page
    gtk_stack_set_visible_child_name(GTK_STACK(app->stack), "page_game");
}

// 2. Called when "START" button is clicked on Login screen
static void on_start_clicked(GtkButton *btn, GameApp *app)
{
    const char *name = gtk_editable_get_text(GTK_EDITABLE(app->name_entry));
    char *trimmed = g_strstrip(g_strdup(name));
    if (g_utf8_strlen(trimmed, -1) == 0) {
        gtk_label_set_text(GTK_LABEL(app->name_warning_label), "Please enter your name to play!");
        g_free(trimmed);
        return;
    }
    gtk_label_set_text(GTK_LABEL(app->name_warning_label), "");
    strncpy(app->player_name, trimmed, sizeof(app->player_name) - 1);
    g_free(trimmed);
    app->player_name[sizeof(app->player_name) - 1] = '\0';
    save_global_settings(app->player_name, -1);
    start_game_logic(app);
}

// 5. Called when "EXIT" button is clicked
static void confirm_exit_response(GObject *source_object, GAsyncResult *res, gpointer user_data) {
    GtkAlertDialog *dialog = GTK_ALERT_DIALOG(source_object);
    GError *error = NULL;
    int response = gtk_alert_dialog_choose_finish(dialog, res, &error);
    if (error) {
        g_error_free(error);
        return;
    }
    if (response == 1) {
        if (return_to_launcher()) {
            GameApp *app = (GameApp *)user_data;
            GtkWindow *window = GTK_WINDOW(app->window);
            GtkApplication *gtk_app = gtk_window_get_application(window);
            g_application_quit(G_APPLICATION(gtk_app));
        }
    }
}

static void on_header_back_clicked(GtkButton *btn, gpointer user_data)
{
    GameApp *app = (GameApp *)user_data;
    const char *visible_child = gtk_stack_get_visible_child_name(GTK_STACK(app->stack));
    if (g_strcmp0(visible_child, "page_game") == 0) {
        GtkAlertDialog *dialog = gtk_alert_dialog_new("Are you sure you want to return to the main menu?");
        gtk_alert_dialog_set_detail(dialog, "Any unsaved progress will be lost.");
        const char *buttons[] = {"Cancel", "Return to Menu", NULL};
        gtk_alert_dialog_set_buttons(dialog, buttons);
        gtk_alert_dialog_set_cancel_button(dialog, 0);
        gtk_alert_dialog_set_default_button(dialog, 0);
        gtk_alert_dialog_choose(dialog, GTK_WINDOW(app->window), NULL, confirm_exit_response, app);
        g_object_unref(dialog);
    } else {
        if (return_to_launcher()) {
            GtkApplication *gtk_app = gtk_window_get_application(GTK_WINDOW(app->window));
            g_application_quit(G_APPLICATION(gtk_app));
        }
    }
}

// 3. Called when "SUBMIT GUESS" button is clicked
static void on_submit_guess(GtkButton *btn, gpointer user_data)
{
    GameApp *app = (GameApp *)user_data; 
    int guess = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(app->guess_spin));

    // Check valid range (1-100)
    if (guess < 1 || guess > 100)
    {
        gtk_label_set_text(GTK_LABEL(app->feedback_label),
                           "Whoa there! That's not in the rulebook.\nOnly numbers from 1 to 100, please!");
        gtk_widget_set_visible(app->feedback_revealer, TRUE);
        return;
    }

    app->attempts++; // Increase attempt counter

    // Update the attempts label text
    char attempts_text[50];
    snprintf(attempts_text, sizeof(attempts_text), "Attempts: %d", app->attempts);
    gtk_label_set_text(GTK_LABEL(app->attempts_label), attempts_text);

    // Check if guess is High, Low, or Correct
    if (guess > app->secret_number)
    {
        gtk_label_set_text(GTK_LABEL(app->feedback_label), "Overshot it! Try a smaller number.");
        gtk_widget_set_visible(app->feedback_revealer, TRUE);
    }
    else if (guess < app->secret_number)
    {
        gtk_label_set_text(GTK_LABEL(app->feedback_label), "That's too low! Aim a little higher!");
        gtk_widget_set_visible(app->feedback_revealer, TRUE);
    }
    else
    {
        // --- WINNER! ---
        
        save_score("number_guessing", app->player_name, app->attempts, 1);
        char best_player[50];
        int best_score = load_top_score("number_guessing", best_player);

        // Prepare result strings
        char congrats[200], numbuf[64], attempts_final[128];
        snprintf(congrats, sizeof(congrats), "Bullseye, %s!", app->player_name);
        snprintf(numbuf, sizeof(numbuf), "The secret number was %d!", app->secret_number);
        
        if (best_score != -1) {
            snprintf(attempts_final, sizeof(attempts_final), "You cracked it in %d guesses!\nAll-Time Best: %s (%d guesses)", app->attempts, best_player, best_score);
        } else {
            snprintf(attempts_final, sizeof(attempts_final), "You cracked it in %d guesses!", app->attempts);
        }

        // Update Result Page Labels
        gtk_label_set_text(GTK_LABEL(app->congrats_label), congrats);
        gtk_label_set_text(GTK_LABEL(app->secret_number_label), numbuf);
        gtk_label_set_text(GTK_LABEL(app->total_attempts_label), attempts_final);

        // Give a fun rank based on attempts
        if (app->attempts == 1)
            gtk_label_set_text(GTK_LABEL(app->praise_label), "IMPOSSIBLE!!! Are you psychic?!");
        else if (app->attempts <= 4)
            gtk_label_set_text(GTK_LABEL(app->praise_label), "Mastermind! Absolute brilliance!");
        else if (app->attempts <= 7)
            gtk_label_set_text(GTK_LABEL(app->praise_label), "Amazing job! You're sharp!");
        else if (app->attempts <= 10)
            gtk_label_set_text(GTK_LABEL(app->praise_label), "Well done! You kept going strong!");
        else
            gtk_label_set_text(GTK_LABEL(app->praise_label), "Phew! You got it just in time!");

        // Show Result Page
        gtk_stack_set_visible_child_name(GTK_STACK(app->stack), "page_result");
    }
}

// 4. Called when "PLAY AGAIN" is clicked
static void on_play_again_clicked(GtkButton *btn, GameApp *app)
{
    start_game_logic(app); // Restart logic
}



// --- UI BUILDER FUNCTIONS ---

// Helper: Creates a standard white card container
GtkWidget *create_card_box(void)
{
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
    gtk_widget_add_css_class(box, "card");
    gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(box, GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(box, 320, -1);
    return box;
}

// Build Page 1: Login Screen
GtkWidget *create_welcome_page(GameApp *app)
{
    GtkWidget *box = create_card_box();

    GtkWidget *welcome_lbl = gtk_label_new("Ready to guess?");
    gtk_widget_add_css_class(welcome_lbl, "welcome-text");

    GtkWidget *q_lbl = gtk_label_new("What's your name, Challenger?");
    gtk_widget_add_css_class(q_lbl, "name-question");

    app->name_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(app->name_entry), "Type your name here...");
    gtk_widget_add_css_class(app->name_entry, "input-field");
    gtk_widget_set_halign(app->name_entry, GTK_ALIGN_CENTER);

    app->name_warning_label = gtk_label_new("");
    gtk_widget_add_css_class(app->name_warning_label, "warning-text");

    GtkWidget *start_btn = gtk_button_new_with_label("START ADVENTURE");
    gtk_widget_add_css_class(start_btn, "btn-blue");
    g_signal_connect(start_btn, "clicked", G_CALLBACK(on_start_clicked), app);

    gtk_box_append(GTK_BOX(box), welcome_lbl);
    gtk_box_append(GTK_BOX(box), q_lbl);
    gtk_box_append(GTK_BOX(box), app->name_entry);
    gtk_box_append(GTK_BOX(box), app->name_warning_label);
    gtk_box_append(GTK_BOX(box), start_btn);

    return box;
}

// Build Page 2: Game Screen
GtkWidget *create_game_page(GameApp *app)
{
    GtkWidget *box = create_card_box();

    GtkWidget *header = gtk_label_new("🎮 NUMBER GUESSING GAME");
    gtk_widget_add_css_class(header, "header-title");

    app->greeting_label = gtk_label_new("Hello, Player!");
    gtk_widget_add_css_class(app->greeting_label, "success-text");

    GtkWidget *instruct = gtk_label_new("What's your first guess?\nTake your best shot!");
    gtk_widget_add_css_class(instruct, "guess-cheer-text");

    // Spinner for selecting numbers
    app->guess_spin = gtk_spin_button_new_with_range(0, 150, 1);
    gtk_widget_add_css_class(app->guess_spin, "input-field");
    gtk_widget_set_halign(app->guess_spin, GTK_ALIGN_CENTER);

    GtkWidget *submit_btn = gtk_button_new_with_label("SUBMIT GUESS");
    gtk_widget_add_css_class(submit_btn, "btn-blue");
    g_signal_connect(submit_btn, "clicked", G_CALLBACK(on_submit_guess), app);

    app->attempts_label = gtk_label_new("Attempts: 0");
    gtk_widget_add_css_class(app->attempts_label, "attempts-text");

    // Revealer box for showing feedback (Too high/low)
    app->feedback_revealer = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    app->feedback_label = gtk_label_new("");
    gtk_widget_add_css_class(app->feedback_revealer, "feedback-box");
    gtk_box_append(GTK_BOX(app->feedback_revealer), app->feedback_label);
    gtk_widget_set_visible(app->feedback_revealer, FALSE); // Start hidden

    gtk_box_append(GTK_BOX(box), header);
    gtk_box_append(GTK_BOX(box), app->greeting_label);
    gtk_box_append(GTK_BOX(box), instruct);
    gtk_box_append(GTK_BOX(box), app->guess_spin);
    gtk_box_append(GTK_BOX(box), submit_btn);
    gtk_box_append(GTK_BOX(box), app->attempts_label);
    gtk_box_append(GTK_BOX(box), app->feedback_revealer);

    // Add developer footer at the very bottom
    GtkWidget *dev_footer = gtk_label_new("Developed by SUJAY PAUL");
    gtk_widget_add_css_class(dev_footer, "dev-footer");
    gtk_box_append(GTK_BOX(box), dev_footer);

    return box;
}

// Build Page 3: Result Screen
GtkWidget *create_result_page(GameApp *app)
{
    GtkWidget *box = create_card_box();

    app->congrats_label = gtk_label_new("CONGRATULATIONS!");
    gtk_widget_add_css_class(app->congrats_label, "game-title");

    GtkWidget *txt1 = gtk_label_new("You guessed the number:");
    gtk_widget_add_css_class(txt1, "result-small-text");

    app->secret_number_label = gtk_label_new("0");
    gtk_widget_add_css_class(app->secret_number_label, "big-number");

    app->total_attempts_label = gtk_label_new("Total Attempts: 0");
    gtk_widget_add_css_class(app->total_attempts_label, "success-text");

    app->praise_label = gtk_label_new("");
    gtk_widget_add_css_class(app->praise_label, "result-performance-text");

    // Horizontal box to hold two buttons side-by-side
    GtkWidget *btn_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_set_halign(btn_box, GTK_ALIGN_CENTER);

    GtkWidget *play_btn = gtk_button_new_with_label("PLAY AGAIN");
    gtk_widget_add_css_class(play_btn, "btn-blue");
    g_signal_connect(play_btn, "clicked", G_CALLBACK(on_play_again_clicked), app);
    g_signal_connect(play_btn, "activate", G_CALLBACK(on_play_again_clicked), app);

    gtk_box_append(GTK_BOX(btn_box), play_btn);

    gtk_box_append(GTK_BOX(box), app->congrats_label);
    gtk_box_append(GTK_BOX(box), txt1);
    gtk_box_append(GTK_BOX(box), app->secret_number_label);
    gtk_box_append(GTK_BOX(box), app->total_attempts_label);
    gtk_box_append(GTK_BOX(box), app->praise_label);
    gtk_box_append(GTK_BOX(box), btn_box);

    // Add developer footer at the very bottom
    GtkWidget *dev_footer = gtk_label_new("Developed by SUJAY PAUL");
    gtk_widget_add_css_class(dev_footer, "dev-footer");
    gtk_box_append(GTK_BOX(box), dev_footer);

    return box;
}

// --- MAIN APPLICATION SETUP ---
// This runs when the app launches
static void activate(GtkApplication *app_system, gpointer user_data)
{
    GameApp *app = g_new0(GameApp, 1); // Create data structure

    // Setup Main Window
    app->window = gtk_application_window_new(app_system);
    gtk_window_set_default_size(GTK_WINDOW(app->window), 900, 700);
    gtk_window_maximize(GTK_WINDOW(app->window));

    GtkWidget *header = gtk_header_bar_new();
    gtk_header_bar_set_show_title_buttons(GTK_HEADER_BAR(header), TRUE);
    gtk_window_set_titlebar(GTK_WINDOW(app->window), header);

    GtkWidget *title_lbl = gtk_label_new("Guess The Number");
    gtk_widget_add_css_class(title_lbl, "header-title");
    gtk_header_bar_set_title_widget(GTK_HEADER_BAR(header), title_lbl);

    // Load CSS Styles
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_string(provider, css_data);
    gtk_style_context_add_provider_for_display(
        gdk_display_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    // Setup "Stack" to switch pages
    app->stack = gtk_stack_new();
    gtk_stack_set_transition_type(GTK_STACK(app->stack),
                                  GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);

    int theme_id;
    load_global_settings(app->player_name, &theme_id);
    // apply_theme(theme_id);

    // Create all pages
    GtkWidget *page1 = create_welcome_page(app);
    GtkWidget *page2 = create_game_page(app);
    GtkWidget *page3 = create_result_page(app);

    // Add pages to the stack
    gtk_stack_add_named(GTK_STACK(app->stack), page1, "page_welcome");
    gtk_stack_add_named(GTK_STACK(app->stack), page2, "page_game");
    gtk_stack_add_named(GTK_STACK(app->stack), page3, "page_result");
    
    // Set default player name if available
    if (strlen(app->player_name) > 0) {
        gtk_editable_set_text(GTK_EDITABLE(app->name_entry), app->player_name);
    }

    if (strlen(app->player_name) > 0 && strcmp(app->player_name, "Player 1") != 0) {
        start_game_logic(app);
    } else {
        gtk_stack_set_visible_child_name(GTK_STACK(app->stack), "page_welcome");
    }

    // Global Overlay for the "Return to Main Menu" button
    GtkWidget *overlay = gtk_overlay_new();
    gtk_overlay_set_child(GTK_OVERLAY(overlay), app->stack);

    GtkWidget *global_btn_back = gtk_button_new_with_label("🔙 Return to Main Menu");
    gtk_widget_set_halign(global_btn_back, GTK_ALIGN_START);
    gtk_widget_set_valign(global_btn_back, GTK_ALIGN_START);
    gtk_widget_set_margin_top(global_btn_back, 15);
    gtk_widget_set_margin_end(global_btn_back, 15);
    gtk_widget_add_css_class(global_btn_back, "btn-blue");
    g_signal_connect(global_btn_back, "clicked", G_CALLBACK(on_header_back_clicked), app);

    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), global_btn_back);

    // Show the overlay in the window
    gtk_window_set_child(GTK_WINDOW(app->window), overlay);
    gtk_window_present(GTK_WINDOW(app->window));
}

// --- PROGRAM ENTRY POINT ---
int main(int argc, char **argv)
{
    GtkApplication *app = gtk_application_new("org.sujay.numberguess", G_APPLICATION_NON_UNIQUE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}