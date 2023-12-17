// https://adventofcode.com/2023/day/3
// gcc -O0 -ggdb -o main_pt1 main_pt1.c -lm
// ./main_pt1 input.txt

#include <math.h>
#include <stdio.h>

#include "common.c"

#define SYMBOL -1
#define DOT -2;

typedef struct {
    int* items;
    size_t n;
    size_t length;
} Matrix_int;

typedef struct {
    int numbers[3];
    int index;
    bool has_adj;
} Number;

int getNumber(const Number* num) {
    int acc = 0;
    printf("Calculating...\n");
    for (size_t i = 0; i < 3; i++) {
        int value = num->numbers[i];
        if (value <= 0) continue;
        int mult = powf(10, num->index - 1 - i);

        printf("\t%d * %d\n", value, mult);
        acc += value * mult;
    }
    return acc;
}

void restartNumber(Number* num) {
    num->has_adj = false;
    num->index = 0;
    for (size_t i = 0; i < 3; i++)
        num->numbers[i] = 0;
}

void pushDigit(Number* num, int value) {
    printf("Pushing %d in index %d\n", value, num->index);
    inner_assert(num->index < 3, "Trying to set a digit");

    num->numbers[num->index] = value;
    num->index++;
}

int* matrix_at(const Matrix_int* matrix, size_t i, size_t j) {
    return (matrix->n * i) + j + matrix->items;
}

Matrix_int create_matrix(size_t n) {
    int* items = calloc(sizeof(int), n * n);
    inner_assert(items != NULL, strerror(errno));
    return (Matrix_int){.items = items, .n = n, .length = n * n};
}

bool has_adj_symbol(Matrix_int matrix, size_t i, size_t j) {
    if (*matrix_at(&matrix, i, j) < 0) return false;

    // top
    if (i >= 1) {
        if (*matrix_at(&matrix, i - 1, j) == SYMBOL)
            return true;
    }

    // bottom
    if (i < (matrix.n - 1)) {
        if (*matrix_at(&matrix, i + 1, j) == SYMBOL)
            return true;
    }

    // left
    if (j >= 1) {
        if (*matrix_at(&matrix, i, j - 1) == SYMBOL)
            return true;
    }

    // right
    if (j < (matrix.n - 1)) {
        if (*matrix_at(&matrix, i, j + 1) == SYMBOL)
            return true;
    }

    // top-left
    if ((i >= 1) && (j >= 1)) {
        if (*matrix_at(&matrix, i - 1, j - 1) == SYMBOL)
            return true;
    }

    // top-right
    if ((i >= 1) && (j < (matrix.n - 1))) {
        if (*matrix_at(&matrix, i - 1, j + 1) == SYMBOL)
            return true;
    }

    // botton-left
    if (i < (matrix.n - 1) && (j >= 1)) {
        if (*matrix_at(&matrix, i + 1, j - 1) == SYMBOL)
            return true;
    }

    // botton-right
    if (i < (matrix.n - 1) && (j < (matrix.n - 1))) {
        if (*matrix_at(&matrix, i + 1, j + 1) == SYMBOL)
            return true;
    }

    return false;
}

int main(int argc, char const* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage:\n    %s <file_path>\n", argv[0]);
        fprintf(stderr, "Error: Invalid number of arguments\n");
        return 1;
    }

    str data = readfile(argv[1]);
    array_view_str lines = split_by((view_str){data.items, data.length}, '\n');
    Matrix_int matrix = create_matrix(lines.length);

    for (size_t i = 0; i < lines.length; i++) {
        view_str line = lines.items[i];
        for (size_t j = 0; j < line.length; j++) {
            if (line.items[j] == '.') {
                *matrix_at(&matrix, i, j) = DOT;
                continue;
            }
            if (isdigit(line.items[j]))
                *matrix_at(&matrix, i, j) = (int)(line.items[j] - '0');
            else
                *matrix_at(&matrix, i, j) = SYMBOL;
        }
    }

    for (size_t i = 0; i < matrix.n; i++) {
        for (size_t j = 0; j < matrix.n; j++) {
            printf("%d\t", *matrix_at(&matrix, i, j));
        }
        printf("\n");
    }
    printf("--------------------------------------------\n");
    int acc = 0;
    Number num = {0};
    for (size_t i = 0; i < matrix.n; i++) {
        for (size_t j = 0; j < matrix.n; j++) {
            int value = *matrix_at(&matrix, i, j);
            if (value < 0) {
                int calc_num = getNumber(&num);
                printf("Num: %d\n", calc_num);
                if (num.has_adj)
                    acc += calc_num;
                restartNumber(&num);
                continue;
            }
            pushDigit(&num, value);
            if (has_adj_symbol(matrix, i, j))
                num.has_adj = true;
        }
    }
    printf("acc: %d\n", acc);
    free(data.items);
    free(lines.items);
    free(matrix.items);
    return 0;
}
