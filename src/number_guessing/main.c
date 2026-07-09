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
#include "../common/persistence.h"
#include "../common/constants.h"
#include "../common/ui_utils.h"
#include "../common/games.h"

#ifdef _WIN32
#include <windows.h>
#endif

// --- GAME DATA ---
// Encapsulates the application's runtime state to avoid globals and support modular UI flows.
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

} NgAppData;

// --- GAME LOGIC FUNCTIONS ---

// Resets the state of the game for a new round
static void ng_start_game_logic(NgAppData *app)
{
    // Determine the secret target (1-100 inclusive)
    app->secret_number = (rand() % 100) + 1;
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

// Triggers when the player starts the game from the login screen
static void ng_on_start_clicked(GtkButton *btn, NgAppData *app)
{
    (void)btn;
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
    ng_start_game_logic(app);
}

static void ng_on_header_back_clicked(GtkButton *btn, gpointer user_data)
{
    (void)btn;
    NgAppData *app = (NgAppData *)user_data;
    (void)app;
    switch_to_launcher();
}

// 3. Called when "SUBMIT GUESS" button is clicked
static void ng_on_submit_guess(GtkButton *btn, gpointer user_data)
{
    (void)btn;
    NgAppData *app = (NgAppData *)user_data; 
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
        int best_score = load_top_score("number_guessing", best_player, sizeof(best_player));

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
static void ng_on_play_again_clicked(GtkButton *btn, NgAppData *app)
{
    (void)btn;
    ng_start_game_logic(app); // Restart logic
}



// --- UI BUILDER FUNCTIONS ---

// Build Page 1: Login Screen
static GtkWidget *ng_create_welcome_page(NgAppData *app)
{
    GtkWidget *box = create_card_box();

    GtkWidget *title_lbl = gtk_label_new("🔢 Number Guessing");
    gtk_widget_add_css_class(title_lbl, "title-large");

    GtkWidget *q_lbl = gtk_label_new("What's your name, Challenger?");
    gtk_widget_add_css_class(q_lbl, "subtitle");

    app->name_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(app->name_entry), "Type your name here...");
    gtk_widget_add_css_class(app->name_entry, "styled-entry");
    gtk_widget_set_halign(app->name_entry, GTK_ALIGN_CENTER);

    app->name_warning_label = gtk_label_new("");
    gtk_widget_add_css_class(app->name_warning_label, "error-msg");

    GtkWidget *start_btn = gtk_button_new_with_label("START ADVENTURE");
    gtk_widget_add_css_class(start_btn, "btn-primary");
    g_signal_connect(start_btn, "clicked", G_CALLBACK(ng_on_start_clicked), app);

    gtk_box_append(GTK_BOX(box), title_lbl);
    gtk_box_append(GTK_BOX(box), q_lbl);
    gtk_box_append(GTK_BOX(box), app->name_entry);
    gtk_box_append(GTK_BOX(box), app->name_warning_label);
    gtk_box_append(GTK_BOX(box), start_btn);

    return box;
}

// Build Page 2: Game Screen
static GtkWidget *ng_create_game_page(NgAppData *app)
{
    GtkWidget *box = create_card_box();

    GtkWidget *header = gtk_label_new("🎮 NUMBER GUESSING GAME");
    gtk_widget_add_css_class(header, "header-title");

    app->greeting_label = gtk_label_new("Hello, Player!");
    gtk_widget_add_css_class(app->greeting_label, "success");

    GtkWidget *instruct = gtk_label_new("What's your first guess?\nTake your best shot!");
    gtk_widget_add_css_class(instruct, "subtitle");

    // Spinner for selecting numbers
    app->guess_spin = gtk_spin_button_new_with_range(0, 150, 1);
    gtk_widget_add_css_class(app->guess_spin, "styled-entry");
    gtk_widget_set_halign(app->guess_spin, GTK_ALIGN_CENTER);

    GtkWidget *submit_btn = gtk_button_new_with_label("SUBMIT GUESS");
    gtk_widget_add_css_class(submit_btn, "btn-primary");
    g_signal_connect(submit_btn, "clicked", G_CALLBACK(ng_on_submit_guess), app);

    app->attempts_label = gtk_label_new("Attempts: 0");
    gtk_widget_add_css_class(app->attempts_label, "score-info");

    // Revealer box for showing feedback (Too high/low)
    app->feedback_revealer = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    app->feedback_label = gtk_label_new("");
    gtk_widget_add_css_class(app->feedback_revealer, "card");
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
    gtk_widget_add_css_class(dev_footer, "footer-credit");
    gtk_box_append(GTK_BOX(box), dev_footer);

    return box;
}

// Build Page 3: Result Screen
static GtkWidget *ng_create_result_page(NgAppData *app)
{
    GtkWidget *box = create_card_box();

    app->congrats_label = gtk_label_new("CONGRATULATIONS!");
    gtk_widget_add_css_class(app->congrats_label, "game-title");

    GtkWidget *txt1 = gtk_label_new("You guessed the number:");
    gtk_widget_add_css_class(txt1, "subtitle");

    app->secret_number_label = gtk_label_new("0");
    gtk_widget_add_css_class(app->secret_number_label, "title-large");

    app->total_attempts_label = gtk_label_new("Total Attempts: 0");
    gtk_widget_add_css_class(app->total_attempts_label, "success");

    app->praise_label = gtk_label_new("");
    gtk_widget_add_css_class(app->praise_label, "score-info");

    // Horizontal box to hold two buttons side-by-side
    GtkWidget *btn_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_set_halign(btn_box, GTK_ALIGN_CENTER);

    GtkWidget *play_btn = gtk_button_new_with_label("PLAY AGAIN");
    gtk_widget_add_css_class(play_btn, "btn-primary");
    g_signal_connect(play_btn, "clicked", G_CALLBACK(ng_on_play_again_clicked), app);
    g_signal_connect(play_btn, "activate", G_CALLBACK(ng_on_play_again_clicked), app);

    gtk_box_append(GTK_BOX(btn_box), play_btn);

    gtk_box_append(GTK_BOX(box), app->congrats_label);
    gtk_box_append(GTK_BOX(box), txt1);
    gtk_box_append(GTK_BOX(box), app->secret_number_label);
    gtk_box_append(GTK_BOX(box), app->total_attempts_label);
    gtk_box_append(GTK_BOX(box), app->praise_label);
    gtk_box_append(GTK_BOX(box), btn_box);

    // Add developer footer at the very bottom
    GtkWidget *dev_footer = gtk_label_new("Developed by SUJAY PAUL");
    gtk_widget_add_css_class(dev_footer, "footer-credit");
    gtk_box_append(GTK_BOX(box), dev_footer);

    return box;
}

// --- MAIN APPLICATION SETUP ---
// This runs when the app launches

GtkWidget* ng_create_ui(void)
{
    static gboolean rng_seeded = FALSE;
    if (!rng_seeded) {
        srand((unsigned)time(NULL));
        rng_seeded = TRUE;
    }

    NgAppData *app = g_new0(NgAppData, 1);

    app->stack = gtk_stack_new();
    gtk_stack_set_transition_type(GTK_STACK(app->stack), GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);

    int theme_id;
    load_global_settings(app->player_name, sizeof(app->player_name), &theme_id);

    GtkWidget *page1 = ng_create_welcome_page(app);
    GtkWidget *page2 = ng_create_game_page(app);
    GtkWidget *page3 = ng_create_result_page(app);

    gtk_stack_add_named(GTK_STACK(app->stack), page1, "page_welcome");
    gtk_stack_add_named(GTK_STACK(app->stack), page2, "page_game");
    gtk_stack_add_named(GTK_STACK(app->stack), page3, "page_result");
    
    if (strlen(app->player_name) > 0) {
        gtk_editable_set_text(GTK_EDITABLE(app->name_entry), app->player_name);
    }

    if (strlen(app->player_name) > 0 && strcmp(app->player_name, "Player 1") != 0) {
        ng_start_game_logic(app);
    } else {
        gtk_stack_set_visible_child_name(GTK_STACK(app->stack), "page_welcome");
    }

    GtkWidget *overlay = gtk_overlay_new();
    gtk_overlay_set_child(GTK_OVERLAY(overlay), app->stack);

    GtkWidget *global_btn_back = gtk_button_new_with_label("🔙 Return to Main Menu");
    gtk_widget_set_halign(global_btn_back, GTK_ALIGN_START);
    gtk_widget_set_valign(global_btn_back, GTK_ALIGN_START);
    gtk_widget_set_margin_top(global_btn_back, 15);
    gtk_widget_set_margin_end(global_btn_back, 15);
    gtk_widget_add_css_class(global_btn_back, "btn-secondary");
    g_signal_connect(global_btn_back, "clicked", G_CALLBACK(ng_on_header_back_clicked), app);

    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), global_btn_back);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    
    GtkWidget *header = gtk_header_bar_new();
    gtk_header_bar_set_show_title_buttons(GTK_HEADER_BAR(header), FALSE);
    GtkWidget *title_lbl = gtk_label_new("Guess The Number");
    gtk_widget_add_css_class(title_lbl, "header-title");
    gtk_header_bar_set_title_widget(GTK_HEADER_BAR(header), title_lbl);
    
    gtk_box_append(GTK_BOX(vbox), header);
    gtk_widget_set_vexpand(overlay, TRUE);
    gtk_box_append(GTK_BOX(vbox), overlay);

    return vbox;
}
