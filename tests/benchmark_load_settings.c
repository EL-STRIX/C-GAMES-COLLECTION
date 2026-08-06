#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "../src/common/persistence.h"

int main(void) {
    char player_name[50];
    int theme_id;

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < 10000; i++) {
        load_global_settings(player_name, sizeof(player_name), &theme_id);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    double time_taken = (double)(end.tv_sec - start.tv_sec) * 1e9;
    time_taken = (time_taken + (double)(end.tv_nsec - start.tv_nsec)) * 1e-9;

    printf("Time taken for 10000 calls: %f seconds\n", time_taken);
    return 0;
}
