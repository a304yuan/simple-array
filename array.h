#ifndef SIMPLE_ARRAY
#define SIMPLE_ARRAY

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct bucket bucket;
typedef struct array array;
typedef struct array_iterator array_iterator;

struct bucket {
    size_t size;
    bucket * next;
    char data[];
};

struct array {
    size_t bucket_capacity;
    size_t size;
    size_t elem_size;
    bucket * bucket_list;
};

struct array_iterator {
    bucket * bck;
    void * next_ref;
    size_t elem_size;
    size_t idx;
    size_t idx_in_bck;
};

extern array * array_new(size_t elem_size, size_t bucket_capacity);
extern void array_free(array * arr);
extern void array_append(array * arr, const void * src);
extern void array_insert(array * arr, size_t idx, const void * src);
extern void array_get(const array * arr, size_t idx, void * dest);
extern void * array_get_ref(const array * arr, size_t idx);
extern void array_set(array * arr, size_t idx, const void * src);
extern void array_delete(array * arr, size_t idx, void * dest);

extern void array_iterator_init(const array * arr, array_iterator * iter);
extern bool array_iterator_has(const array_iterator * iter);
extern void array_iterator_next(array_iterator * iter, void * dest);
extern void * array_iterator_next_ref(array_iterator * iter);

#endif /* end of include guard: SIMPLE-ARRAY */
