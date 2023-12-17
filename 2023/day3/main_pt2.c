// https://adventofcode.com/2023/day/3

#include <math.h>
#include <stdio.h>

#include "common.c"

typedef struct {
    int digit;
    size_t i;
    size_t j;
} Digit;

typedef struct {
    Digit digits[3];
    size_t next_digit_index;
    bool has_adj;
} Number;

typedef struct {
    size_t i;
    size_t j;
} Gear;

typedef struct {
    Number* items;
    size_t length;
    size_t capacity;
} array_number;

typedef struct {
    Gear* items;
    size_t length;
    size_t capacity;
} array_gear;

int getNumber(const Number* num) {
    int acc = 0;
    // printf("Calculating...\n");
    for (size_t i = 0; i < 3; i++) {
        Digit digit = num->digits[i];
        if (digit.digit < 1) continue;
        int mult = powf(10, num->next_digit_index - 1 - i);
        // printf("\t%d * %d\n", digit.digit, mult);
        acc += digit.digit * mult;
    }
    return acc;
}

void restartNumber(Number* num) {
    num->has_adj = false;
    num->next_digit_index = 0;
    for (size_t i = 0; i < 3; i++)
        num->digits[i] = (Digit){0};
}

void pushDigit(Number* num, char digit, size_t i, size_t j) {
    // printf("Pushing %c in index %d\n", digit, num->next_digit_index);
    inner_assert(num->next_digit_index < 3, "Trying to set more than three digit");
    inner_assert(isdigit(digit), "Invalid digit");

    num->digits[num->next_digit_index] = (Digit){.digit = digit - '0', .i = i, .j = j};
    num->next_digit_index++;
}

bool is_adj(size_t row_1, size_t col_1, size_t row_2, size_t col_2) {
    size_t drow = 0;
    if (row_1 > row_2)
        drow = (row_1 - row_2);
    else
        drow = (row_2 - row_1);

    size_t dcol = 0;
    if (col_1 > col_2)
        dcol = (col_1 - col_2);
    else
        dcol = (col_2 - col_1);

    /*
        row | col | result
        ----|-----|-------
        0   | 0   | F     <- Same point
        0   | 1   | V     <- Next/Prev. col
        1   | 0   | V     <- Next/Prev. row
        1   | 1   | V     <- In Diagonal
        X   | X   | F     <- Not adjecent
    */
    return (drow <= 1 && dcol <= 1) && (drow == 1 || dcol == 1);
}

int gear_ratio(Gear gear, array_number numbers) {
    size_t count_adj = 0;
    int gear_ratio = 1;
    for (size_t i = 0; i < numbers.length; i++) {
        Number number = numbers.items[i];
        for (size_t j = 0; j < 3; j++) {
            Digit digit = number.digits[j];
            if (is_adj(gear.i, gear.j, digit.i, digit.j)) {
                count_adj++;
                gear_ratio *= getNumber(&number);
                break;
            }
        }
        if (count_adj > 2)
            return 0;
    }
    if (count_adj < 2)
        return 0;
    return gear_ratio;
}

int main(int argc, char const* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage:\n    %s <file_path>\n", argv[0]);
        fprintf(stderr, "Error: Invalid number of arguments\n");
        return 1;
    }

    str data = readfile(argv[1]);
    array_view_str lines = split_by((view_str){data.items, data.length}, '\n');
    array_number numbers = {0};
    array_gear gears = {0};
    Number number = {0};

    for (size_t i = 0; i < lines.length; i++) {
        view_str line = lines.items[i];
        for (size_t j = 0; j < line.length; j++) {
            if (isdigit(line.items[j]))
                pushDigit(&number, line.items[j], i, j);
            else {
                if (number.next_digit_index > 0)
                    push(&numbers, &number);
                restartNumber(&number);
                if (line.items[j] == '*') {
                    push(&gears, (&(Gear){i, j}));
                }
            }
        }
    }

    int acc = 0;
    for (size_t i = 0; i < gears.length; i++) {
        int ratio = gear_ratio(gears.items[i], numbers);
        printf("Gear raio (%lu %lu) : %d\n", gears.items[i].i, gears.items[i].j, ratio);
        acc += ratio;
    }

    printf("acc: %d\n", acc);
    free(data.items);
    free(lines.items);
    free(gears.items);
    free(numbers.items);

    return 0;
}
