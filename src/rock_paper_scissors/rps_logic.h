#ifndef RPS_LOGIC_H
#define RPS_LOGIC_H

/**
 * Evaluates the result of a Rock Paper Scissors round.
 *
 * @param user_choice The user's choice (1 = Rock, 2 = Paper, 3 = Scissors)
 * @param computer_choice The computer's choice (1 = Rock, 2 = Paper, 3 = Scissors)
 * @return 0 for a draw, 1 if player wins, 2 if computer wins
 */
int rps_evaluate_round(int user_choice, int computer_choice);

#endif /* RPS_LOGIC_H */
