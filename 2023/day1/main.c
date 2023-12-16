// https://adventofcode.com/2023/day/1
// Build: gcc -O0 -ggdb main.c -o main
// Run: ./main path/to/file

#include "common.c"

typedef struct {
    view_str viewstr;
    int digit;
} item_t;

typedef struct {
    size_t position;
    int digit;
} digit_t;

typedef struct {
    digit_t* items;
    size_t length;
} array_digit_t;

const item_t TABLE1[] = {
    {VIEW_STR("one"), 1},   {VIEW_STR("two"), 2},   {VIEW_STR("three"), 3},
    {VIEW_STR("four"), 4},  {VIEW_STR("five"), 5},  {VIEW_STR("six"), 6},
    {VIEW_STR("seven"), 7}, {VIEW_STR("eight"), 8}, {VIEW_STR("nine"), 9}};

const item_t TABLE2[] = {{VIEW_STR("1"), 1}, {VIEW_STR("2"), 2}, {VIEW_STR("3"), 3},
                         {VIEW_STR("4"), 4}, {VIEW_STR("5"), 5}, {VIEW_STR("6"), 6},
                         {VIEW_STR("7"), 7}, {VIEW_STR("8"), 8}, {VIEW_STR("9"), 9}};

#define TABLE_LENGTH (sizeof(TABLE1) / sizeof(TABLE1[0]))

int comp_digit(const void* elem1, const void* elem2) {
    digit_t d1 = *((digit_t*)elem1);
    digit_t d2 = *((digit_t*)elem2);
    if (d1.position > d2.position) return 1;
    if (d1.position < d2.position) return -1;
    return 0;
}

array_digit_t check_line(view_str line) {
    list_t list = {.item_size = sizeof(digit_t)};
    for (size_t i = 0; i < TABLE_LENGTH; i++) {
        array_size_t positions = view_str_contains_all(line, TABLE2[i].viewstr);
        for (size_t j = 0; j < positions.length; j++) {
            digit_t digit = {.position = positions.items[j], .digit = TABLE2[i].digit};
            push(&list, &(digit));
        }
        free(positions.items);

        positions = view_str_contains_all(line, TABLE1[i].viewstr);
        for (size_t j = 0; j < positions.length; j++) {
            digit_t digit = {.position = positions.items[j], .digit = TABLE1[i].digit};
            push(&list, &(digit));
        }
        free(positions.items);
    }
    if (list.length < 1) return (array_digit_t){0};

    array_digit_t ret = {list.items, list.length};
    qsort(ret.items, list.length, sizeof(digit_t), comp_digit);
    return ret;
}

int main(int argc, char const* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Error: Inform at least one file path!\n");
        return 1;
    }

    str data = readfile(argv[1]);
    array_view_str lines = split_by((view_str){data.items, data.length}, '\n');

    int total = 0;
    for (size_t i = 0; i < lines.length; i++) {
        view_str line = lines.items[i];
        array_digit_t digits = check_line(line);
        if (digits.length < 1) continue;

        printf("%.*s: [", (int)line.length, line.items);
        for (size_t j = 0; j < digits.length; j++)
            printf("%d (%lu), ", digits.items[j].digit, digits.items[j].position);
        printf("] : ");

        digit_t item_begin = digits.items[0];
        digit_t item_end = digits.items[digits.length - 1];
        int value = item_begin.digit * 10 + item_end.digit;
        printf("%d\n", value);
        total += value;
        free(digits.items);
    }
    free(lines.items);
    free(data.items);
    printf("Total: %d\n", total);
    return 0;
}
