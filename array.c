#include "array.h"

struct _elem_position {
    bucket * bck;
    size_t idx;
};

static bucket * array_bucket_new(size_t elem_size, size_t capacity) {
    bucket * bck = malloc(sizeof(bucket) + capacity * elem_size);
    bck->size = 0;
    bck->next = NULL;
    return bck;
}

static struct _elem_position array_find(const array * arr, size_t idx) {
    bucket * cur = arr->bucket_list;
    bucket * prev = NULL;
    while (idx >= cur->size) {
        // clean empty bucket
        if (cur->size == 0 && prev) {
            prev->next = cur->next;
            free(cur);
        }
        idx -= cur->size;
        prev = cur;
        cur = cur->next;
    }
    struct _elem_position pos = {.bck = cur, .idx = idx};
    return pos;
}

array * array_new(size_t elem_size, size_t bucket_capacity) {
    array * arr = malloc(sizeof(array));
    arr->bucket_capacity = bucket_capacity;
    arr->elem_size = elem_size;
    arr->size = 0;
    arr->bucket_list = array_bucket_new(elem_size, bucket_capacity);
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

void array_append(array * arr, const void * src) {
    bucket * bck = arr->bucket_list;
    while (bck->next) {
        bck = bck->next;
    }
    if (bck->size == arr->bucket_capacity) {
        bucket * new_bck = array_bucket_new(arr->elem_size, arr->bucket_capacity);
        bck->next = new_bck;
        bck = new_bck;
    }
    memcpy(bck->data + bck->size * arr->elem_size, src, arr->elem_size);
    bck->size += 1;
    arr->size += 1;
}

void array_insert(array * arr, size_t idx, const void * src) {
    if (idx >= arr->size) return;

    struct _elem_position pos = array_find(arr, idx);
    bucket * bck = pos.bck;
    if (bck->size == arr->bucket_capacity) {
        bucket * new_bck = array_bucket_new(arr->elem_size, arr->bucket_capacity);
        memcpy(new_bck->data, bck->data + pos.idx * arr->elem_size, (bck->size - pos.idx) * arr->elem_size);
        new_bck->next = bck->next;
        new_bck->size = bck->size - pos.idx;
        bck->next = new_bck;
        bck->size = pos.idx;
    }
    memcpy(bck->data + pos.idx * arr->elem_size, src, arr->elem_size);
    bck->size += 1;
    arr->size += 1;
}

void array_get(const array * arr, size_t idx, void * dest) {
    if (idx >= arr->size) return;

    struct _elem_position pos = array_find(arr, idx);
    memcpy(dest, pos.bck->data + pos.idx * arr->elem_size, arr->elem_size);
}

void * array_get_ref(const array * arr, size_t idx) {
    if (idx >= arr->size) return NULL;

    struct _elem_position pos = array_find(arr, idx);
    return pos.bck->data + pos.idx * arr->elem_size;
}

void array_set(array * arr, size_t idx, const void * src) {
    if (idx >= arr->size) return;

    struct _elem_position pos = array_find(arr, idx);
    memcpy(pos.bck->data + pos.idx * arr->elem_size, src, arr->elem_size);
}

void array_delete(array * arr, size_t idx, void * dest) {
    if (idx >= arr->size) return;

    struct _elem_position pos = array_find(arr, idx);
    bucket * bck = pos.bck;
    if (dest) {
        memcpy(dest, bck->data + pos.idx * arr->elem_size, arr->elem_size);
    }
    memmove(bck->data + pos.idx * arr->elem_size, bck->data + (pos.idx + 1) * arr->elem_size, (bck->size - pos.idx - 1) * arr->elem_size);
    bck->size -= 1;
    arr->size -= 1;
}

void array_iter_init(const array * arr, array_iter * iter) {
    iter->bck = arr->bucket_list;
    iter->elem_size = arr->elem_size;
    iter->next_ref = arr->bucket_list->data;
    iter->idx = 0;
    iter->idx_in_bck = 0;
}

bool array_iter_has(const array_iter * iter) {
    return iter->bck && iter->idx_in_bck < iter->bck->size;
}

size_t array_iter_next(array_iter * iter, void * dest) {
    memcpy(dest, iter->next_ref, iter->elem_size);
    if (++iter->idx_in_bck == iter->bck->size) {
        iter->bck = iter->bck->next;
        iter->next_ref = iter->bck->data;
        iter->idx_in_bck = 0;
    }
    else {
        iter->next_ref += iter->elem_size;
    }
    return iter->idx++;
}

size_t array_iter_next_ref(array_iter * iter, void ** dest) {
    *dest = iter->next_ref;
    if (++iter->idx_in_bck == iter->bck->size) {
        iter->bck = iter->bck->next;
        if (iter->bck) {
            iter->next_ref = iter->bck->data;
        }
        iter->idx_in_bck = 0;
    }
    else {
        iter->next_ref += iter->elem_size;
    }
    return iter->idx++;
}
