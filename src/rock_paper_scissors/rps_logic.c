#include "rps_logic.h"

int rps_evaluate_round(int user_choice, int computer_choice) {
    if (user_choice == computer_choice) {
        return 0;
    } else if ((user_choice == 1 && computer_choice == 3) ||
               (user_choice == 2 && computer_choice == 1) ||
               (user_choice == 3 && computer_choice == 2)) {
        return 1;
    } else {
        return 2;
    }
}
