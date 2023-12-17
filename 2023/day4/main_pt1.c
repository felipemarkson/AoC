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

int main(int argc, char const* argv[]) {
    inner_assert(argc == 2, "Invalid number of arguments");
    str data = readfile(argv[1]);
    array_view_str lines = split_by((view_str){data.items, data.length}, '\n');

    int total = 0;
    for (size_t i = 0; i < lines.length; i++) {
        view_str line = lines.items[i];
        line.items += strlen("Card XXX: ");
        line.length -= strlen("Card XXX: ");

        array_view_str rows = split_by(line, '|');
        array_int win_nums = parse_row(rows.items[0]);
        array_int chose_nums = parse_row(rows.items[1]);

        int points = 0;
        for (size_t j = 0; j < chose_nums.length; j++) {
            int chose_num = chose_nums.items[j];
            for (size_t k = 0; k < win_nums.length; k++) {
                int win_num = win_nums.items[k];
                if (chose_num == win_num) {
                    points = points == 0 ? 1 : points * 2;
                    break;
                }
            }
        }

        printf("points: %d\n", points);
        total += points;

        free(rows.items);
        free(win_nums.items);
        free(chose_nums.items);
    }
     printf("Total: %d\n", total);
    free(data.items);
    free(lines.items);

    return 0;
}
