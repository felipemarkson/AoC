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

typedef struct {
    void* items;
    size_t item_size;
    size_t capacity;
    size_t length;
} list_t;

void set_capacity(list_t* list, size_t capacity) {
    unsigned char* new_item = realloc(list->items, list->item_size * capacity);
    inner_assert(new_item != NULL, strerror(errno));
    list->items = new_item;
    list->capacity = capacity;
}

void* at(list_t* list, size_t i) {
    if (i >= list->length) return NULL;
    unsigned char* as_bytes = list->items;
    return &(as_bytes[i * list->item_size]);
}
void clear(list_t* list) {
    memset(list->items, 0, list->item_size * list->capacity);
    list->length = 0;
}

void push(list_t* list, const void* item) {
    inner_assert(list->capacity >= list->length, "Invalid usage of list_t");

    if (list->capacity == list->length) {
        size_t new_capacity = list->capacity == 0 ? 2 : (list->capacity * 2);
        set_capacity(list, new_capacity);
    }
    list->length = list->length + 1;
    void* dest = at(list, list->length - 1);
    memcpy(dest, item, list->item_size);
}

void pop(list_t* list, void* dest) {
    if (dest != NULL) {
        void* out = at(list, list->length - 1);
        memcpy(dest, out, list->item_size);
    }
    list->length = list->length - 1;
}

typedef struct {
    char* items;
    size_t length;
} str;

typedef struct {
    const char* items;
    size_t length;
} view_str;

typedef struct {
    view_str* items;
    size_t length;
} array_view_str;

typedef struct {
    size_t* items;
    size_t length;
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
    list_t list = {.item_size = sizeof(size_t)};
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
    return (array_size_t){.items = list.items, .length = list.length};
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
    return (str){.items = data, .length = file_size};
}

array_view_str split_by(view_str data, char delim) {
    list_t list = {.item_size = sizeof(view_str)};
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

    return (array_view_str){list.items, list.length};
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

view_str trim(view_str data){
    return ltrim(rtrim(data));
}