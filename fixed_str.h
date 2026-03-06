#ifndef FIXED_STR_H_
#define FIXED_STR_H_

#include <stddef.h>

typedef struct {
    char* items;
    size_t len;
} fs_t;

extern fs_t* fs_init(size_t len);
extern fs_t* fs_slice(const fs_t* str, size_t start, size_t end);
extern size_t fs_len(fs_t* str, size_t start);
extern void fs_t_free(fs_t* str);

#ifdef FIXED_STR_IMPLEMENTATION

fs_t* fs_init(size_t len)
{
    fs_t* str = (fs_t*)calloc(1, sizeof(*str));
    str->items = (char*)calloc(len, sizeof(char*));
    str->len = len;
    return str;
}

// upto end 
fs_t* fs_slice(const fs_t* str, size_t start, size_t end)
{
    assert(str != NULL && "[ERROR] str is null");
    size_t slice_len = end - start;
    fs_t* slice = fs_init(slice_len + 1);
    for (size_t i = 0; i < slice_len; i++) {
        slice->items[i] = str->items[start + i];
    }
    slice->items[slice_len] = 0;
    return slice;
}

size_t fs_len(fs_t* str, size_t start)
{
    assert(str != NULL && "[ERROR] array is null");
    size_t len = 0;
    while(str->items[start + len] != 0)
        len++;
    return len;
}

void fs_free(fs_t* str)
{
    assert(str != NULL && "[ERROR] array is null");
    free(str->items);
    free(str);
}
#endif // FIXED_STR_IMPLEMENTATION
#endif // FIXED_STR_H__
