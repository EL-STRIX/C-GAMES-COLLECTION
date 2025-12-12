#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Board row and columns
char board[3][3];

// Initialize the board with characters '1'..'9'
void init_board(void)
{
    char num = '1';
    for (int r = 0; r < 3; ++r)
        for (int c = 0; c < 3; ++c)
            board[r][c] = num++;
}

// Print the current board state to console
void print_board()
{
    for (int r = 0; r < 3; ++r)
    {
        printf(" %c | %c | %c \n", board[r][0], board[r][1], board[r][2]);
        if (r < 2)
            printf("---+---+---\n");
    }
}

// Check the winning
int check_winner()
{
    // Check for row
    for (int r = 0; r < 3; r++)
    {
        if (board[r][0] == board[r][1] && board[r][1] == board[r][2])
            return 1;
    }
    // Check for colums
    for (int c = 0; c < 3; c++)
    {
        if (board[0][c] == board[1][c] && board[1][c] == board[2][c])
            return 1;
    }
    // Check for diagonals
    if (board[0][0] == board[1][1] && board[1][1] == board[2][2])
    {
        return 1;
    }
    if (board[0][2] == board[1][1] && board[1][1] == board[2][0])
    {
        return 1;
    }
}

//Check the draw
int check_draw(){
    for(int r=0; r<3; r++){
        for(int c=0; c<3; c++){
            if(board[r][c] != 'X' && board[r][c] != 'O'){
                return 0;
            }
        }
    }
    return 1;
}

// Merk the location
void mark_board(char choise, int cell_no, char player_name[])
{
    int chance = 0;
    while (1)
    {
        int r = (cell_no - 1) / 3;
        int c = (cell_no - 1) % 3;
        if (board[r][c] == 'X' || board[r][c] == 'O')
        {
            printf("The cell number is already selected!");
            printf("\nPlayer %s please enter another cell number: ", player_name);
            scanf("%d", &cell_no);
        }
        else
        {
            break;
        }
        chance++;
        if (chance == 3)
        {
            printf("\nYOU PEAC PF BITCH! \nYOU HAVE NOT KNOW A SIMPLE GAME RULES! \nGO DIE!!\n\n");
            break;
        }
    }
    int r = (cell_no - 1) / 3;
    int c = (cell_no - 1) % 3;
    board[r][c] = choise;
}

// Chose player and cell number
void chose_player(char name1[], char name2[])
{
    int player = 1;
    while (1)
    {
        print_board();
        char player_name[30];
        char choise;
        int cell_no;

        if (player % 2 != 0)
        {
            choise = 'X';
            strcpy(player_name, name1);
        }
        else
        {
            choise = 'O';
            strcpy(player_name, name2);
        }

        printf("\nPlayer %s enter your cell number(1-9): ", player_name);
        scanf("%d", &cell_no);
        while (cell_no < 1 || cell_no > 9)
        {
            printf("Invalid cell number!");
            printf("\nplease enter the cell number again: ");
            scanf("%d", &cell_no);
        }

        mark_board(choise, cell_no, player_name);

        int win = check_winner();
        if (win == 1)
        {
            print_board();
            printf("\nCONGRATULATION! %s", player_name);
            printf("\nYOU WON THE MATCH");
            break;
        }

        int draw = check_draw();
        if(draw==1){
            printf("\nTHE MATCH IS DRAW!");
            break;
        }

        player++;
    }
}

int main()
{
    char name1[20];
    char name2[20];
    char play_again;

    // Take player1 name
    printf("\nENTER THE 1st PLAYER NAME: ");
    fgets(name1, sizeof(name1), stdin);
    name1[strcspn(name1, "\n")] = 0;

    // If name is empty
    while (strlen(name1) == 0)
    {
        printf("NAME CANNOT BE EMPTY. ENTER THE 1st PLAYER NAME: ");
        fgets(name1, sizeof(name1), stdin);
        name1[strcspn(name1, "\n")] = 0;
    }

    // Take player2 name
    printf("\nENTER THE 2nd PLAYER NAME: ");
    fgets(name2, sizeof(name2), stdin);
    name2[strcspn(name2, "\n")] = 0;

    // If name is empty
    while (strlen(name2) == 0)
    {
        printf("NAME CANNOT BE EMPTY. ENTER THE 2nd PLAYER NAME: ");
        fgets(name2, sizeof(name2), stdin);
        name2[strcspn(name2, "\n")] = 0;
    }

    // Some fonts to make the game like proffesional
    printf("\nHEY %s AND %s", name1, name2);
    printf("\nWELLCOME TO THE TIC-TAC-TOE GAME!\n");
    printf("A GAME OF REFLEX AND LUCK!\n\n");

    do
    {
        // Function calling to play the game
        init_board();

        chose_player(name1, name2);

        // Play again
        printf("\n\nDo you want to play again?\n");
        printf("Enter \"Y\" for 'YES'\n");
        printf("Enter \"N\" for 'NO'\n");
        printf("Enter your choice: ");
        while (getchar() != '\n')
            ;

        // Loop to continue or exit the game
        while (1)
        {

            // take input from user
            play_again = getchar();
            while (getchar() != '\n')
                ;

            if (play_again == 'Y' || play_again == 'y' || play_again == 'N' || play_again == 'n')
            {
                break;
            }
            else
            {
                printf("Invalid!! Please enter \"Y\" and \"N\": ");
            }
        }

    } while (play_again == 'Y' || play_again == 'y');

    printf("Thanks for playing %s AND %s !Goodbye!\n", name1, name2);
}