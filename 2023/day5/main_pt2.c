// gcc -O0 -ggdb -Wall -Wextra -o main_pt1 main_pt1.c

#include <stdio.h>

#include "common.c"

typedef struct {
    size_t source;
    size_t dest;
    size_t size;
} Map;

typedef struct {
    Map* items;
    size_t length;
    size_t capacity;
} array_Map;

typedef struct {
    size_t* items;
    size_t length;
    size_t capacity;
} array_int;

size_t map_value(const Map* map, size_t value) {
    if ((value >= map->source) & (value < map->source + map->size)) {
        return (value - map->source) + map->dest;
    }
    return value;
}

size_t map_value_in_array(const array_Map* set, size_t value) {
    for (size_t i = 0; i < set->length; i++) {
        size_t mapped = map_value(set->items + i, value);
        if (mapped != value) return mapped;
    }
    return value;
}

view_str pop_until(view_str* data, view_str token) {
    view_str out = *data;
    for (size_t i = 0; i < (data->length - token.length); i++) {
        view_str part = {.items = data->items + i, .length = token.length};
        if (view_str_eq(part, token)) {
            out.length = i;
            data->items += i + token.length;
            data->length -= i + token.length;
            return out;
        }
    }

    return out;
}

array_int parse_seeds(view_str* vdata) {  // Seeds
    array_int seeds = {0};
    view_str chunk = pop_until(vdata, (view_str)VIEW_STR("\n\n"));
    chunk.items += strlen("seeds: ");
    chunk.length -= strlen("seeds: ");
    view_str chunk_number = {0};
    while (chunk_number.items != chunk.items) {
        chunk_number = pop_until(&chunk, (view_str)VIEW_STR(" "));
        size_t seed = strtoul(chunk_number.items, NULL, 10);
        // printf("%d\n", seed);
        push(&seeds, &seed);
    }
    return seeds;
}

array_Map parse_map(view_str* data) {
    array_Map maps = {0};
    view_str chunk = pop_until(data, (view_str)VIEW_STR("\n\n"));
    // printf("%.*s\n", chunk.length, chunk.items);
    (void)pop_until(&chunk, (view_str)VIEW_STR("\n"));  // skip header
    view_str row = {0};
    while (row.items != chunk.items) {
        row = pop_until(&chunk, (view_str)VIEW_STR("\n"));
        view_str row_copy = row;
        view_str dest_str = pop_until(&row_copy, (view_str)VIEW_STR(" "));
        view_str source_str = pop_until(&row_copy, (view_str)VIEW_STR(" "));
        view_str size_str = pop_until(&row_copy, (view_str)VIEW_STR(" "));
        Map map = {
            .dest = strtol(dest_str.items, NULL, 10),
            .source = strtol(source_str.items, NULL, 10),
            .size = strtol(size_str.items, NULL, 10),
        };
        push(&maps, &map);
    }
    return maps;
}

#define NUM_MAPS 7
#define FLAG __SIZE_MAX__
int main(int argc, char const* argv[]) {
    inner_assert(argc == 2, "Invalid number of parameters. Inform only the file path");

    printf("WARNING: THIS CODE COULD TAKE A LOT OF TIME!!!!\n");

    str data = readfile(argv[1]);
    view_str vdata = (view_str){data.items, data.length};
    array_Map maps[NUM_MAPS] = {0};

    array_int seeds = parse_seeds(&vdata);
    for (size_t i = 0; i < NUM_MAPS; i++) {
        maps[i] = parse_map(&vdata);
    }
    size_t min_map = __SIZE_MAX__;
    for (size_t i = 0; i < seeds.length; i += 2) {
        size_t seed_start = seeds.items[i];
        size_t seed_length = seeds.items[i + 1];
        size_t count = 0;
        for (size_t seed = seed_start; seed < seed_start + seed_length; seed++) {
            size_t mapped = seed;
            for (size_t j = 0; j < NUM_MAPS; j++) {
                mapped = map_value_in_array(&(maps[j]), mapped);
            }
            if (min_map > mapped)
                min_map = mapped;
            if (count % 1000000 == 0)
                printf(" Seed: %lu | Mapped: %lu\n", seed, mapped);
            count++;
        }
    }
    printf("Min mapped: %lu\n", min_map);
    for (size_t j = 0; j < NUM_MAPS; j++) {
        free(maps[j].items);
    }
    free(seeds.items);
    free(data.items);

    return 0;
}
