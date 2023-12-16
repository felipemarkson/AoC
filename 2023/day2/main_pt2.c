// https://adventofcode.com/2023/day/2
// Build: gcc -O0 -ggdb main_pt2.c -o main_pt2
// Run: ./main_pt2 path/to/file

#include <stdio.h>

#include "common.c"

typedef struct
{
    int green;
    int blue;
    int red;
} Cube;

typedef struct {
    Cube* items;
    size_t length;
    int id;
} Array_Cube;

Array_Cube get_array_cube(view_str line) {
    Array_Cube out = {0};
    list_t list_cubes = {.item_size = sizeof(Cube)};
    array_view_str tokens = split_by(line, ':');
    view_str token_1 = tokens.items[0];
    view_str token_2 = tokens.items[1];

    {  // get game id
        array_view_str tokens_game = split_by(token_1, ' ');
        view_str game = tokens_game.items[1];
        out.id = strtol(game.items, NULL, 10);
        free(tokens_game.items);
    }

    {  // get cubes
        array_view_str tokens_game = split_by(token_2, ';');
        for (size_t i = 0; i < tokens_game.length; i++) {
            view_str cube_str = tokens_game.items[i];
            Cube cube = {0};
            {  // get color
                array_view_str face_colors = split_by(cube_str, ',');
                for (size_t j = 0; j < face_colors.length; j++) {
                    view_str face_color = trim(face_colors.items[j]);
                    int value = strtol(face_color.items, NULL, 10);
                    if (view_str_contains(face_color, (view_str)VIEW_STR("blue"), NULL)) {
                        cube.blue = value;
                    } else if (view_str_contains(face_color, (view_str)VIEW_STR("red"), NULL)) {
                        cube.red = value;
                    } else if (view_str_contains(face_color, (view_str)VIEW_STR("green"), NULL)) {
                        cube.green = value;
                    } else {
                        inner_assert(false, "Invalidade name");
                    }
                }
                free(face_colors.items);
            }
            push(&list_cubes, &cube);
        }
        free(tokens_game.items);
    }

    out.items = list_cubes.items;
    out.length = list_cubes.length;

    free(tokens.items);
    return out;
}

void print_game(const Array_Cube* game) {
    printf("Game %d: {\n", game->id);
    for (size_t i = 0; i < game->length; i++) {
        Cube cube = game->items[i];
        printf("   {red: %d, green: %d, blue: %d},\n", cube.red, cube.green, cube.blue);
    }
    printf("}\n");
}

int main(int argc, char const* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage:\n    %s <file_path>\n", argv[0]);
        fprintf(stderr, "Error: Invalid number of arguments\n");
        return 1;
    }

    str data = readfile(argv[1]);
    array_view_str lines = split_by((view_str){data.items, data.length}, '\n');

    int power = 0;
    for (size_t i = 0; i < lines.length; i++) {
        if (lines.items[i].length < 1) continue;
        Array_Cube game = get_array_cube(lines.items[i]);
        print_game(&game);

        Cube cube_minimum = {0};
        for (size_t j = 0; j < game.length; j++) {
            Cube cube = game.items[j];
            if (cube.blue > cube_minimum.blue) cube_minimum.blue = cube.blue;
            if (cube.red > cube_minimum.red) cube_minimum.red = cube.red;
            if (cube.green > cube_minimum.green) cube_minimum.green = cube.green;
        }
        int power_cube = cube_minimum.blue * cube_minimum.green * cube_minimum.red;
        printf("    Power: %d\n", power_cube);
        power += power_cube;

        free(game.items);
    }

    printf("Sum of minimum powers: %d\n", power);
    free(lines.items);
    free(data.items);
    return 0;
}
