// gcc -O0 -ggdb -Wall -Wextra -o main_pt2 -lm main_pt2.c
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
size_t calc_solutions(size_t total_time, size_t record) {
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
    size_t result = calc_solutions(40929790, 215106415051100);
    printf("Result: %lu\n", result);
    return 0;
}
