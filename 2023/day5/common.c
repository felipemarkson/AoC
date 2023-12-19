#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef inner_assert
#define inner_assert(expr, cstr_msg_err)                                         \
    do {                                                                         \
        if (!(expr)) {                                                           \
            fprintf(stderr, "FATAL ERROR!\n   %s:%d\n   %s\n   Caused by: %s\n", \
                    __FILE__, __LINE__, (cstr_msg_err), #expr);                  \
            exit(1);                                                             \
        }                                                                        \
    } while (0)
#endif

#define set_capacity(list_ptr, new_capacity)                                                           \
    do {                                                                                               \
        (list_ptr)->items = realloc((list_ptr)->items, sizeof((list_ptr)->items[0]) * (new_capacity)); \
        inner_assert((list_ptr)->items != NULL, strerror(errno));                                      \
        (list_ptr)->capacity = (new_capacity);                                                         \
    } while (0)

#define clear(list_ptr)                                                                    \
    do {                                                                                   \
        memset((list_ptr)->items, 0, sizeof((ptr_list)->items[0]) * (list_ptr)->capacity); \
        (list_ptr)->length = 0;                                                            \
    } while (0)

#define at(list_ptr, index) ((list_ptr)->items + (index))

#define push(list_ptr, item_ptr)                                                                   \
    do {                                                                                           \
        inner_assert((list_ptr)->capacity >= (list_ptr)->length, "Invalid usage of (list_ptr)_t"); \
        if ((list_ptr)->capacity == (list_ptr)->length)                                            \
            set_capacity((list_ptr), (list_ptr)->capacity == 0 ? 64 : ((list_ptr)->capacity * 2)); \
        (list_ptr)->length = (list_ptr)->length + 1;                                               \
        memcpy(at(list_ptr, (list_ptr)->length - 1), (item_ptr), sizeof((list_ptr)->items[0]));    \
    } while (0)

#define pop(list_ptr, dest_ptr)                                                                       \
    do {                                                                                              \
        if ((dest_ptr) != NULL)                                                                       \
            memcpy((dest_ptr), at((list_ptr), (list_ptr)->length - 1), sizeof((list_ptr)->items[0])); \
        (list_ptr)->length = (list_ptr)->length - 1;                                                  \
    } while (0)

typedef struct {
    char* items;
    size_t length;
    size_t capacity;
} str;

typedef struct {
    const char* items;
    size_t length;
} view_str;

typedef struct {
    view_str* items;
    size_t length;
    size_t capacity;
} array_view_str;

typedef struct {
    size_t* items;
    size_t length;
    size_t capacity;
} array_size_t;

#define VIEW_STR(cstr) \
    { .items = (const char*)(cstr), .length = strlen((cstr)) }

bool view_str_eq(view_str first, view_str second) {
    if (first.length != second.length) return false;
    for (size_t i = 0; i < first.length; i++)
        if (first.items[i] != second.items[i]) return false;
    return true;
}

bool view_str_contains(view_str haystack, view_str needle, size_t* first_position) {
    if (needle.length > haystack.length) return false;
    for (size_t i = 0; i < haystack.length; i++) {
        bool equal = false;
        for (size_t j = 0; (j < needle.length); j++) {
            if (!(j < (haystack.length - i))) {
                equal = false;
                break;
            }
            if (haystack.items[i + j] == needle.items[j]) {
                equal = true;
            } else {
                equal = false;
                break;
            }
        }
        if (equal) {
            if (first_position) *first_position = i;
            return true;
        }
    }
    return false;
}

array_size_t view_str_contains_all(view_str haystack, view_str needle) {
    array_size_t list = {0};
    if (needle.length > haystack.length) return (array_size_t){0};
    for (size_t i = 0; i < haystack.length; i++) {
        bool equal = false;
        for (size_t j = 0; (j < needle.length); j++) {
            if (!(j < (haystack.length - i))) {
                equal = false;
                break;
            }
            if (haystack.items[i + j] == needle.items[j]) {
                equal = true;
            } else {
                equal = false;
                break;
            }
        }
        if (equal) push(&list, &i);
    }
    return list;
}

str readfile(const char* path) {
    FILE* file = fopen(path, "r");
    long file_size = 0;
    char* data = 0;

    inner_assert(file, "Could not open the file");
    inner_assert(fseek(file, 0, SEEK_END) == 0, strerror(errno));
    inner_assert((file_size = ftell(file)) > 0, strerror(errno));
    inner_assert(fseek(file, 0, SEEK_SET) == 0, strerror(errno));
    inner_assert((data = malloc(file_size + 1)), strerror(errno));
    inner_assert(fread(data, file_size, 1, file) != (size_t)file_size, "fread fail");
    fclose(file);
    data[file_size] = '\0';
    return (str){.items = data, .length = file_size, .capacity = file_size};
}

array_view_str split_by(view_str data, char delim) {
    array_view_str list = {0};
    size_t len = 0;
    for (size_t i = 0; i < data.length; i++) {
        if (data.items[i] == delim) {
            view_str str = {.items = &(data.items[i - len]), .length = len};
            push(&list, &(str));
            len = 0;
        } else {
            ++len;
        }
    }
    view_str str = {.items = &(data.items[data.length - len]), .length = len};
    push(&list, &(str));

    return list;
}

view_str ltrim(view_str data) {
    view_str out = {0};
    for (size_t i = 0; i < data.length; i++) {
        if (!isspace(data.items[i])) {
            out.items = &(data.items[i]);
            out.length = data.length - i;
            break;
        }
    }
    return out;
}

view_str rtrim(view_str data) {
    view_str out = {.items = data.items};
    for (size_t i = 0; i < data.length; i++) {
        size_t j = data.length - (i + 1);
        if (!isspace(data.items[j])) {
            out.length = data.length - i;
            break;
        }
    }
    return out;
}

view_str trim(view_str data) {
    return ltrim(rtrim(data));
}