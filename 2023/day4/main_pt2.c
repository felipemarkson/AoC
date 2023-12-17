#include <stdio.h>

#include "common.c"

typedef struct {
    int* items;
    size_t length;
    size_t capacity;
} array_int;

array_int parse_row(view_str row) {
    row = trim(row);
    array_view_str numbers_as_str = split_by(row, ' ');
    array_int nums = {0};
    for (size_t j = 0; j < numbers_as_str.length; j++) {
        view_str number_str = trim(numbers_as_str.items[j]);
        if (number_str.length < 1) continue;
        int number = strtol(number_str.items, NULL, 10);
        push(&nums, &number);
    }
    free(numbers_as_str.items);
    return nums;
}

void calc_match(int* qnt_card, size_t index, const array_int* arr_win, const array_int* arr_chose, size_t len) {
    array_int win_nums = arr_win[index];
    array_int chose_nums = arr_chose[index];

    size_t count = 0;
    for (size_t i = 0; i < chose_nums.length; i++) {
        int chose_num = chose_nums.items[i];
        for (size_t j = 0; j < win_nums.length; j++) {
            int win_num = win_nums.items[j];
            if (chose_num == win_num) {
                ++count;
                break;
            }
        }
    }

    for (size_t i = index; (i < index + count) & (i < len - 1); i++) {
        qnt_card[i + 1] += 1;
        calc_match(qnt_card, i + 1, arr_win, arr_chose, len);
    }
}

int main(int argc, char const* argv[]) {
    inner_assert(argc == 2, "Invalid number of arguments");
    str data = readfile(argv[1]);
    array_view_str lines = split_by((view_str){data.items, data.length}, '\n');

    array_int* arr_win = calloc(lines.length, sizeof(array_int));
    array_int* arr_chose = calloc(lines.length, sizeof(array_int));
    int* qnt_card = calloc(lines.length, sizeof(int));

    for (size_t i = 0; i < lines.length; i++) {
        view_str line = lines.items[i];
        line.items += strlen("Card XXX: ");
        line.length -= strlen("Card XXX: ");

        array_view_str rows = split_by(line, '|');
        arr_win[i] = parse_row(rows.items[0]);
        arr_chose[i] = parse_row(rows.items[1]);

        free(rows.items);
    }

    for (size_t i = 0; i < lines.length; i++) {
        qnt_card[i] += 1;
        calc_match(qnt_card, i, arr_win, arr_chose, lines.length);
    }

    int total = 0;
    for (size_t i = 0; i < lines.length; i++) {
        printf("Card %lu: %d\n", i + 1, qnt_card[i]);
        total += qnt_card[i];
        free(arr_win[i].items);
        free(arr_chose[i].items);
    }

    printf("Total:  %d\n", total);
    free(qnt_card);
    free(arr_win);
    free(arr_chose);
    free(data.items);
    free(lines.items);

    return 0;
}
