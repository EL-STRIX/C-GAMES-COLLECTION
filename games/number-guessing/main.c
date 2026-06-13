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

const char *css_data =
    "window { background-color: #cbcbcb; }"
    "button { all: unset; border-radius: 8px; padding: 10px; }"
    ".card { background-color: white; border-radius: 15px; padding: 20px; margin: 25px; box-shadow: 0 4px 8px rgba(0,0,0,0.1); }"
    ".header-title { font-size: 25px; font-weight: bold; color: #5208ff; margin-bottom: 10px; }"
    ".name-question { font-size: 18px; font-weight: 500; color: #444; margin-bottom: 15px; }"
    ".welcome-text { font-size: 26px; font-weight: 800; color: #279cf5; margin-bottom: 20px; }"
    ".input-field { font-size: 18px; padding: 10px; margin-bottom: 20px; border-radius: 8px; }"
    ".guess-cheer-text { font-size: 18px; font-weight: 600; color: #444; }"
    ".btn-blue { background-color: #1e40af; color: white; font-weight: bold; padding: 10px; border-radius: 8px; }"
    ".btn-blue:hover { background-color: #3b82f6; }"
    ".feedback-box { background-color: #fee2e2; color: #991b1b; border-radius: 8px; padding: 10px; margin-top: 15px; font-weight: bold; }"
    ".success-text { font-size: 20px; font-weight: bold; color: #1e293b; }"
    ".big-number { font-size: 25px; font-weight: 900; color: #22c55e; margin: 10px; }"
    ".tip-text { color: #666; font-size: 15px; margin-top: 20px; }"
    ".warning-text { color: #ff3333; font-weight: 700; font-size: 15px; margin-top: 5px; }"
    ".attempts-text { color: #0f3edd; font-size: 20px; font-weight: 700; }"
    ".result-small-text { font-size: 20px; font-weight: 600; color: #444; }"
    ".result-performance-text { font-size: 22px; font-weight: 700; color: rgba(9, 94, 223, 0.47); margin-top: 10px; }"
    ".dev-footer { font-size: 14px; color: #666; font-weight: 600; }";

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

// 2. Called when "START" button is clicked
static void on_start_clicked(GtkButton *btn, GameApp *app)
{
    // Get the text inside the name entry box
    const char *name = gtk_editable_get_text(GTK_EDITABLE(app->name_entry));

    // Check if the name is empty
    if (strlen(name) == 0)
    {
        gtk_label_set_text(GTK_LABEL(app->name_warning_label),
                           "Whoops! I need a name to cheer for. Please enter one!");
        gtk_widget_set_visible(app->name_warning_label, TRUE); // Show error
        return;
    }

    // Name is good: Hide error, save name, start game
    gtk_widget_set_visible(app->name_warning_label, FALSE);
    strncpy(app->player_name, name, 99);

    start_game_logic(app);
}

// 3. Called when "SUBMIT GUESS" button is clicked
static void on_submit_guess(GtkButton *btn, GameApp *app)
{
    // Get the number from the spinner
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

        // Prepare result strings
        char congrats[200], numbuf[64], attempts_final[64];
        snprintf(congrats, sizeof(congrats), "Bullseye, %s!", app->player_name);
        snprintf(numbuf, sizeof(numbuf), "The secret number was %d!", app->secret_number);
        snprintf(attempts_final, sizeof(attempts_final), "You cracked it in %d guesses!", app->attempts);

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

// 5. Called when "EXIT GAME" is clicked
static void on_exit_clicked(GtkButton *btn, GameApp *app)
{
    // Close the window and quit the app
    g_application_quit(G_APPLICATION(gtk_window_get_application(GTK_WINDOW(app->window))));
}

// --- UI BUILDER FUNCTIONS ---

// Helper: Creates a standard white card container
GtkWidget *create_card_box()
{
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
    gtk_widget_add_css_class(box, "card"); // Use .card CSS class
    gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(box, GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(box, 320, -1);
    return box;
}

// Build Page 1: Welcome Screen
GtkWidget *create_welcome_page(GameApp *app)
{
    GtkWidget *box = create_card_box();

    GtkWidget *header = gtk_label_new("🎮 NUMBER GUESSING GAME");
    gtk_widget_add_css_class(header, "header-title");

    GtkWidget *welcome = gtk_label_new("Welcome!");
    gtk_widget_add_css_class(welcome, "welcome-text");

    GtkWidget *instruction = gtk_label_new("What should I call you, challenger?");
    gtk_widget_add_css_class(instruction, "name-question");

    // Name Input Field
    app->name_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(app->name_entry), "Enter your legendary name...");
    gtk_widget_add_css_class(app->name_entry, "input-field");
    g_signal_connect(app->name_entry, "activate", G_CALLBACK(on_start_clicked), app);

    // Warning Label (Hidden by default)
    app->name_warning_label = gtk_label_new("");
    gtk_widget_add_css_class(app->name_warning_label, "warning-text");
    gtk_widget_set_visible(app->name_warning_label, FALSE);

    // Start Button
    GtkWidget *start_btn = gtk_button_new_with_label("START");
    gtk_widget_add_css_class(start_btn, "btn-blue");
    g_signal_connect(start_btn, "clicked", G_CALLBACK(on_start_clicked), app);

    GtkWidget *tip = gtk_label_new("💡 Tip: Hey hero… don’t forget to tell me who you are!");
    gtk_widget_add_css_class(tip, "tip-text");

    // Add everything to the box
    gtk_box_append(GTK_BOX(box), header);
    gtk_box_append(GTK_BOX(box), welcome);
    gtk_box_append(GTK_BOX(box), instruction);
    gtk_box_append(GTK_BOX(box), app->name_entry);
    gtk_box_append(GTK_BOX(box), start_btn);
    gtk_box_append(GTK_BOX(box), app->name_warning_label);
    gtk_box_append(GTK_BOX(box), tip);

    // Add developer footer at the very bottom
    GtkWidget *dev_footer = gtk_label_new("Developed by SUJAY PAUL");
    gtk_widget_add_css_class(dev_footer, "dev-footer");
    gtk_box_append(GTK_BOX(box), dev_footer);

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
    gtk_widget_add_css_class(app->congrats_label, "header-title");

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

    GtkWidget *exit_btn = gtk_button_new_with_label("EXIT GAME");
    gtk_widget_add_css_class(exit_btn, "btn-blue");
    g_signal_connect(exit_btn, "clicked", G_CALLBACK(on_exit_clicked), app);

    gtk_box_append(GTK_BOX(btn_box), play_btn);
    gtk_box_append(GTK_BOX(btn_box), exit_btn);

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
    gtk_window_set_title(GTK_WINDOW(app->window), "Guess The Number");
    gtk_window_set_default_size(GTK_WINDOW(app->window), 400, 500);

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

    // Create all pages
    GtkWidget *page1 = create_welcome_page(app);
    GtkWidget *page2 = create_game_page(app);
    GtkWidget *page3 = create_result_page(app);

    // Add pages to the stack
    gtk_stack_add_named(GTK_STACK(app->stack), page1, "page_welcome");
    gtk_stack_add_named(GTK_STACK(app->stack), page2, "page_game");
    gtk_stack_add_named(GTK_STACK(app->stack), page3, "page_result");

    // Show the stack in the window
    gtk_window_set_child(GTK_WINDOW(app->window), app->stack);
    gtk_window_present(GTK_WINDOW(app->window));
}

// --- PROGRAM ENTRY POINT ---
int main(int argc, char **argv)
{
    GtkApplication *app;
    int status;

    app = gtk_application_new("com.example.numberguess", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}