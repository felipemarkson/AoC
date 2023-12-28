// gcc -O0 -ggdb -Wall -Wextra -o main_pt1 -lm main_pt1.c
#include <math.h>
#include <stdint.h>
#include <stdio.h>

#include "common.c"

/* Equations:

    S = Tp(Tt - Tp)
    S(Tp) = -Tp² + TtTp (1)

    S(Tp) > Sr
    -Tp² + TtTp > Sr
    Tp² - TtTp + Sr < 0
    Tp0 = (Tt +- (Tt² - 4Sr)^0.5)/2 (2)
*/
unsigned int calc_solutions(unsigned int total_time, unsigned int record) {
    double delta = (((double)total_time) * ((double)total_time)) - 4.0L * ((double)record);
    if (delta <= 0.0L)
        return 0;

    double x2 = ((((double)total_time) + sqrt(delta)) / 2.0L);
    double x1 = ((((double)total_time) - sqrt(delta)) / 2.0L);

    if (x1 < 0.0L)
        x1 = 0.0L;
    if (x2 < 0.0L)
        x2 = 0.0L;

    double x2_floor = floor(x2);
    if (x2 == x2_floor)
        x2_floor -= 1.0L;
    double x1_ceil = ceil(x1);
    if (x1 == x1_ceil)
        x1_ceil += 1.0L;
    return x2_floor - x1_ceil + 1;
}

int main(void) {
    unsigned int race[4] = {0};
    race[0] = calc_solutions(40, 215);
    race[1] = calc_solutions(92, 1064);
    race[2] = calc_solutions(97, 1505);
    race[3] = calc_solutions(90, 1100);

    unsigned int acc = 1;
    for (size_t i = 0; i < 4; i++) {
        acc *= race[i];
        printf("Solution %lu: %u\n", i, race[i]);
    }

    printf("Result: %u\n", acc);
    return 0;
}
