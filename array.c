#include "array.h"

struct _elem_position {
    bucket ** bck;
    size_t idx;
};

static bucket * array_bucket_new(size_t capacity) {
    bucket * bck = malloc(sizeof(bucket) + capacity * sizeof(any));
    bck->size = 0;
    bck->next = NULL;
    return bck;
}

static struct _elem_position array_find(const array * arr, size_t idx) {
    bucket ** cur = (bucket**)&arr->bucket_list;
    while (idx >= (*cur)->size) {
        idx -= (*cur)->size;
        cur = &(*cur)->next;
    }
    return (struct _elem_position){.bck = cur, .idx = idx};
}

array * array_new(size_t bucket_capacity) {
    array * arr = malloc(sizeof(array));
    arr->bucket_capacity = bucket_capacity;
    arr->size = 0;
    arr->bucket_list = array_bucket_new(bucket_capacity);
    return arr;
}

void array_free(array * arr) {
    bucket * bck = arr->bucket_list;
    while (bck) {
        bucket * next = bck->next;
        free(bck);
        bck = next;
    }
    free(arr);
}

void array_append(array * arr, any val) {
    bucket * bck = arr->bucket_list;
    while (bck->next) {
        bck = bck->next;
    }
    if (bck->size == arr->bucket_capacity) {
        bucket * new_bck = array_bucket_new(arr->bucket_capacity);
        bck->next = new_bck;
        bck = new_bck;
    }
    bck->data[bck->size] = val;
    bck->size += 1;
    arr->size += 1;
}

void array_insert(array * arr, size_t idx, any val) {
    if (idx >= arr->size) return;

    struct _elem_position pos = array_find(arr, idx);
    bucket * bck = *pos.bck;
    if (bck->size == arr->bucket_capacity) {
        bucket * new_bck = array_bucket_new(arr->bucket_capacity);
        memcpy(new_bck->data, bck->data + pos.idx, (bck->size - pos.idx) * sizeof(any));
        new_bck->next = bck->next;
        new_bck->size = bck->size - pos.idx;
        bck->next = new_bck;
        bck->size = pos.idx;
    }
    else {
        for (size_t i = bck->size - 1; i >= pos.idx; i--) {
            bck->data[i+1] = bck->data[i];
        }
    }
    bck->data[pos.idx] = val;
    bck->size += 1;
    arr->size += 1;
}

any array_get(const array * arr, size_t idx) {
    if (idx >= arr->size) return (any){0};

    struct _elem_position pos = array_find(arr, idx);
    return (*pos.bck)->data[pos.idx];
}

any * array_get_ref(const array * arr, size_t idx) {
    if (idx >= arr->size) return NULL;

    struct _elem_position pos = array_find(arr, idx);
    return (*pos.bck)->data + pos.idx;
}

void array_set(array * arr, size_t idx, any val) {
    if (idx >= arr->size) return;

    struct _elem_position pos = array_find(arr, idx);
    (*pos.bck)->data[pos.idx] = val;
}

void array_delete(array * arr, size_t idx) {
    if (idx >= arr->size) return;

    struct _elem_position pos = array_find(arr, idx);
    bucket * bck = *pos.bck;
    //memmove(bck->data + pos.idx, bck->data + pos.idx + 1, (bck->size - pos.idx - 1) * sizeof(any));
    for (size_t i = pos.idx; i + 1 < bck->size; i++) {
        bck->data[i] = bck->data[i+1];
    }
    bck->size -= 1;
    arr->size -= 1;

    // clean empty bucket
    if (bck->size == 0) {
        *pos.bck = bck->next;
        free(bck);
    }
}

void array_iter_init(const array * arr, array_iter * iter) {
    iter->bck = arr->bucket_list;
    iter->idx = 0;
}

bool array_iter_has(const array_iter * iter) {
    return iter->bck && iter->idx < iter->bck->size;
}

any * array_iter_next_ref(array_iter * iter) {
    any * e = iter->bck->data + iter->idx;
    if (iter->idx == iter->bck->size - 1) {
        iter->bck = iter->bck->next;
        iter->idx = 0;
    }
    else {
        iter->idx++;
    }
    return e;
}

any array_iter_next(array_iter * iter) {
    return *array_iter_next_ref(iter);
}
