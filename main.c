#include <stdio.h>
#include <string.h>

// Game board represented as a 3x3 2D array
char board[3][3];

//Clear input buffer to prevent scanf issues
void clear_input_buffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Only accept single digit 1-9, reject letters/symbols/special characters
int get_valid_cell_number()
{
    int cell_no;
    
    while (1)
    {
        if (scanf("%d", &cell_no) != 1)
        {
            clear_input_buffer();
            printf("Invalid input! Only numbers 1-9 are allowed. Please try again: ");
            continue;
        }
        
        // Check if there are extra characters after the number
        char next_char = getchar();
        if (next_char != '\n' && next_char != EOF)
        {
            clear_input_buffer();
            printf("Invalid input! Only single digit 1-9 allowed. Please try again: ");
            continue;
        }
        
        // Validate range 1-9
        if (cell_no < 1 || cell_no > 9)
        {
            printf("Invalid cell number! Must be between 1-9. Please try again: ");
            continue;
        }
        
        return cell_no;
    }
}

/*
  Initialize the board with numbers 1-9
  These numbers represent the cell positions players can choose
*/
void init_board(void)
{
    char num = '1';
    for (int r = 0; r < 3; ++r)
    {
        for (int c = 0; c < 3; ++c)
        {
            board[r][c] = num++;
        }
    }
}

// Display the current state of the game board

void print_board()
{
    for (int r = 0; r < 3; ++r)
    {
        printf(" %c | %c | %c \n", board[r][0], board[r][1], board[r][2]);
        if (r < 2)
        {
            printf("---+---+---\n");
        }
    }
}

/*
  Check if there is a winner
  Returns 1 if a player has won, 0 otherwise
*/
int check_winner()
{
    // Check rows
    for (int r = 0; r < 3; r++)
    {
        if (board[r][0] == board[r][1] && board[r][1] == board[r][2])
            return 1;
    }
    
    // Check columns
    for (int c = 0; c < 3; c++)
    {
        if (board[0][c] == board[1][c] && board[1][c] == board[2][c])
            return 1;
    }
    
    // Check diagonal (top-left to bottom-right)
    if (board[0][0] == board[1][1] && board[1][1] == board[2][2])
    {
        return 1;
    }
    
    // Check diagonal (top-right to bottom-left)
    if (board[0][2] == board[1][1] && board[1][1] == board[2][0])
    {
        return 1;
    }

    return 0;
}

/*
  Check if the game is a draw
  Returns 2 if all cells are filled (draw), 0 otherwise
*/
int check_draw()
{
    for (int r = 0; r < 3; r++)
    {
        for (int c = 0; c < 3; c++)
        {
            // If any cell still has a number (not X or O), game continues
            if (board[r][c] != 'X' && board[r][c] != 'O')
            {
                return 0;
            }
        }
    }
    return 2;
}

/*
  Mark a cell on the board with X or O
  Validates the move and gives the player up to 3 attempts for invalid moves
  
  Parameters:
    choice - 'X' or 'O'
    cell_no - Cell number from 1-9
    player_name - Name of the current player
*/
void mark_board(char choice, int cell_no, char player_name[])
{
    int attempts = 0; // store the time of invalid marking on board by player
    // If player chose invalid cell for 3 time, the player loses his chance to play
    
    while (1)
    {
        // Convert cell number to array indices
        int r = (cell_no - 1) / 3; // Determine the row index in 2D array
        int c = (cell_no - 1) % 3; // Determine the column index in 2D array
        
        // Check if cell is already occupied
        if (board[r][c] == 'X' || board[r][c] == 'O')
        {
            printf("The cell number is already selected!");
            printf("\nPlayer %s please enter another cell number: ", player_name);
            cell_no = get_valid_cell_number();
            attempts++;
            
            // After 3 invalid attempts, skip the player's turn
            if (attempts == 2)
            {
                printf("\nToo many invalid attempts!\n");
                printf("Turn skipped. Please follow the game rules carefully.\n\n");
                return; // skip marking and lose the turn
            }
        }
        else
        {
            break;
        }
    }
    
    // Convert cell number to array indices
    int r = (cell_no - 1) / 3; // Determine the row index in 2D array
    int c = (cell_no - 1) % 3; // Determine the column index in 2D array

    // Mark the cell with O/X
    board[r][c] = choice;
}

/*
  Main game loop - handles player turns and determines round winner
  
  Parameters:
    name1 - Player 1's name
    name2 - Player 2's name
  
  Returns:
    1 if Player 1 wins
    2 if Player 2 wins
    0 if the round is a draw
*/
int choose_player(char name1[], char name2[])
{
    int player = 1;
    
    while (1)
    {
        print_board(); // print the game board
        char player_name[30]; // Create another variable to copy the current player name
        char choice; // Define the mark(O/X)
        int cell_no; // For marking O/X

        // Calculate the current player using even, odd method
        if (player % 2 != 0)
        {
            // For player 1
            choice = 'X';
            strcpy(player_name, name1);
        }
        else
        {
            // For player 2
            choice = 'O';
            strcpy(player_name, name2);
        }

        // Get player's move
        printf("\nPlayer %s enter your cell number(1-9): ", player_name);
        cell_no = get_valid_cell_number();

        // Mark the game board with O/X in the chosen cell number
        mark_board(choice, cell_no, player_name);

        // Check all possibilities of winning the game
        int win = check_winner();
        if (win == 1)
        {
            print_board();
            printf("\nCONGRATULATIONS! %s", player_name);
            printf("\nYOU WON THIS ROUND");
            
            if (player % 2 != 0)
            {
                return 1; // Player 1 win this round
            }
            else
            {
                return 2; // Player 2 win this round
            }
        }

        // Check all cell fill or not if all cell filled it declare draw the match
        int draw = check_draw();
        if (draw == 2)
        {
            print_board();
            printf("\nTHE MATCH IS DRAW!");
            return 0; // Return 0 for match draw
            break;
        }

        // Switch to next player
        player++;
    }
}

int main()
{
    char name1[20]; // Player name 1
    char name2[20]; // Player name 2
    char play_again; // Take char (y/n) for play again

    // Get Player 1's name
    printf("\nENTER THE 1st PLAYER NAME: ");
    fgets(name1, sizeof(name1), stdin);
    name1[strcspn(name1, "\n")] = 0; // Remove newline character

    while (strlen(name1) == 0)
    {
        printf("NAME CANNOT BE EMPTY. ENTER THE 1st PLAYER NAME: ");
        fgets(name1, sizeof(name1), stdin);
        name1[strcspn(name1, "\n")] = 0;
    }

    // Get Player 2's name
    printf("\nENTER THE 2nd PLAYER NAME: ");
    fgets(name2, sizeof(name2), stdin);
    name2[strcspn(name2, "\n")] = 0;

    while (strlen(name2) == 0)
    {
        printf("NAME CANNOT BE EMPTY. ENTER THE 2nd PLAYER NAME: ");
        fgets(name2, sizeof(name2), stdin);
        name2[strcspn(name2, "\n")] = 0;
    }

    // Welcome message
    printf("\nHEY %s AND %s", name1, name2);
    printf("\nWELCOME TO THE TIC-TAC-TOE GAME!\n");
    printf("A GAME OF REFLEX AND LUCK!");

    // Score tracking
    int player1_score = 0;
    int player2_score = 0;
    // Store final result of winning
    int result = 0;

    do
    {
        // To play 3 rounds
        int round = 0;
        while (round < 3)
        {
            printf("\n\nROUND %d\n\n", round + 1);
            // initialize the main board
            init_board();

            // Give chance player 1 and 2 equally
            result = choose_player(name1, name2);

            // Update scores based on round result
            if (result == 1)
            {
                player1_score++;
            }
            else if (result == 2)
            {
                player2_score++;
            }

            // Increase round
            round++;
        }

        // Show final result after 3 rounds
        printf("\n\n%s score is: %d", name1, player1_score);
        printf("\n%s score is: %d", name2, player2_score);

        // Determine overall winner
        if (player1_score > player2_score)
        {
            printf("\n\nWINNER: %s\n", name1);
        }
        else if (player1_score < player2_score)
        {
            printf("\n\nWINNER: %s\n", name2);
        }
        else
        {
            printf("\n\nMATCH DRAW!\n");
        }

        // Ask if players want to play again
        printf("\n\nDo you want to play again?\n");
        printf("Enter \"Y\" for 'YES'\n");
        printf("Enter \"N\" for 'NO'\n");
        printf("Enter your choice: ");
        
        // Clear buffer properly
        clear_input_buffer();

        // Get valid input (Y/N)
        while (1)
        {
            play_again = getchar();
            // Clear buffer properly
            clear_input_buffer();

            if (play_again == 'Y' || play_again == 'y' || play_again == 'N' || play_again == 'n')
            {
                break;
            }
            else
            {
                printf("Invalid!! Please enter \"Y\" and \"N\": ");
            }
        }

        // Reset scores for a new game
        if (play_again == 'Y' || play_again == 'y')
        {
            player1_score = 0;
            player2_score = 0;
        }

    } while (play_again == 'Y' || play_again == 'y');

    printf("Thanks for playing %s AND %s! Goodbye!\n", name1, name2);
}