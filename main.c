#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>

void play_game(char name[]) {
   //Some fonts to make the game like proffesional
    printf("\nHEY %s",name);
    printf("\nWELLCOME TO THE SNAKE-GUN-WATER GAME!\n");
    printf("A GAME OF REFLEX AND LUCK!\n\n");
    printf("---GAME INSTRUCTIONS---\n");
    printf("ENTER 1 FOR SNAKE\n");
    printf("ENTER 2 FOR GUN\n");
    printf("ENTER 3 FOR WATER\n");
    printf("MAKE YOUR MOVE AND TRY TO BEAT THE COMPUTER!\n\n");

    //Loop for codition check
    int user_choice=0, computer_win=0, player_win=0,i=1;
    while (i<=3)
    {
        //Print the round numbers
        printf("ROUND NUMBER %d\n",i);

        //Genarate a random number by computer
        int computer_choice;
        srand(time(0));
        computer_choice = (rand()%3)+1;

        //Take choice from user
        printf("ENTER YOUR CHOICE: ");
        scanf("%d",&user_choice);

        if (user_choice==1 || user_choice==2 || user_choice==3) {

            //Print user choice
            if (user_choice==1)
                printf("\nYOU CHOOSE SNAKE.");
            else if (user_choice==2)
                printf("\nYOU CHOOSE GUN.");
            else if (user_choice==3)
                printf("\nYOU CHOOSE WATER.");

            //Print computer choice
            if (computer_choice==1)
                printf("\nCOMPUTER CHOOSE SNAKE.\n\n");
            else if (computer_choice==2)
                printf("\nCOMPUTER CHOOSE GUN.\n\n");
            else if (computer_choice==3)
                printf("\nCOMPUTER CHOOSE WATER.\n\n");

            //Print the result
            if ((user_choice==1 && computer_choice==1)||(user_choice==2 && computer_choice==2)||(user_choice==3 && computer_choice==3)) {
                printf("IT'S DRAW! TRY AGAIN.\n\n");
                i++;
            }
            else if ((user_choice==1 && computer_choice==3)||(user_choice==2 && computer_choice==1)||(user_choice==3 && computer_choice==2)) {
                printf("YOU WIN! GREAT CHOICE!\n\n");
                player_win++;
                i++;
            }
            else if ((user_choice==1 && computer_choice==2)||(user_choice==2 && computer_choice==3)||(user_choice==3 && computer_choice==1)) {
                printf("YOU LOSE! BETTER LUCK NEXT TIME!\n\n");
                computer_win++;
                i++;
            }
        }
        else {
            printf("\nCHOOCE A VALID CHOICE!\n\n");
        }

    }

    //Give the final result after 3 round
    printf("\nGAME OVER!\n");
    if (player_win>computer_win) {
        printf("CONGRATULATION!\n");
        printf("YOU WIN THE GAME!\n");
        printf("YOUR POINT IS %d AND THE COMPUTER POINT IS %d.",player_win,computer_win);
    }
    else if (player_win<computer_win) {
        printf("\nCOMPUTER WIN THE GAME!\n");
        printf("YOUR POINT IS %d AND THE COMPUTER POINT IS %d.",player_win,computer_win);
    } else {
        printf("\nTHE GAME IS A DRAW!\n");
        printf("YOUR POINT IS %d AND THE COMPUTER POINT IS %d.",player_win,computer_win);
    }
}

int main()
{
    char name[30];
    char play_again;
    do {
        //Take player name
        printf("\nENTER YOUR NAME: ");
        fgets(name, sizeof(name), stdin);
        name[strcspn(name, "\n")] = 0;

        // If name is empty
        while (strlen(name) == 0) {
            printf("NAME CANNOT BE EMPTY. ENTER YOUR NAME: ");
            fgets(name, sizeof(name), stdin);
            name[strcspn(name, "\n")] = 0;
        }

        //Function calling to play the game
        play_game(name);

        //Play again
        printf("\n\nDo you want to play again?\n");
        printf("Enter \"Y\" for 'YES'\n");
        printf("Enter \"N\" for 'NO'\n");
        printf("Enter your choice: ");
        while (getchar() != '\n');


        //Loop to continue or exit the game
        while (1)
        {

            //take input from user
            play_again = getchar();
            while (getchar() != '\n');

            if(play_again == 'Y' || play_again == 'y' || play_again == 'N' || play_again == 'n')
            {
                break;
            }
            else
            {
                printf("Invalid!! Please enter \"Y\" and \"N\": ");
            }
        }

    } while (play_again == 'Y' || play_again == 'y');

    printf("Thanks for playing %s!Goodbye!\n", name);

}