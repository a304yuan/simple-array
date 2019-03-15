#include "array.h"

struct _elem_position {
    bucket * bck;
    size_t idx;
};

static bucket * array_bucket_new(size_t capacity) {
    bucket * bck = malloc(sizeof(bucket) + capacity * sizeof(any));
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
            cur = prev->next;
            continue;
        }
        idx -= cur->size;
        prev = cur;
        cur = cur->next;
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
    bucket * bck = pos.bck;
    if (bck->size == arr->bucket_capacity) {
        bucket * new_bck = array_bucket_new(arr->bucket_capacity);
        memcpy(new_bck->data, bck->data + pos.idx, (bck->size - pos.idx) * sizeof(any));
        new_bck->next = bck->next;
        new_bck->size = bck->size - pos.idx;
        bck->next = new_bck;
        bck->size = pos.idx;
    }
    bck->data[pos.idx] = val;
    bck->size += 1;
    arr->size += 1;
}

any array_get(const array * arr, size_t idx) {
    if (idx >= arr->size) return (any){0};

    struct _elem_position pos = array_find(arr, idx);
    return pos.bck->data[pos.idx];
}

any * array_get_ref(const array * arr, size_t idx) {
    if (idx >= arr->size) return NULL;

    struct _elem_position pos = array_find(arr, idx);
    return pos.bck->data + pos.idx;
}

void array_set(array * arr, size_t idx, any val) {
    if (idx >= arr->size) return;

    struct _elem_position pos = array_find(arr, idx);
    pos.bck->data[pos.idx] = val;
}

void array_delete(array * arr, size_t idx) {
    if (idx >= arr->size) return;

    struct _elem_position pos = array_find(arr, idx);
    bucket * bck = pos.bck;
    memmove(bck->data + pos.idx, bck->data + pos.idx + 1, (bck->size - pos.idx - 1) * sizeof(any));
    bck->size -= 1;
    arr->size -= 1;
}

void array_iter_init(const array * arr, array_iter * iter) {
    iter->bck = arr->bucket_list;
    iter->next_ref = arr->bucket_list->data;
    iter->idx = 0;
    iter->idx_in_bck = 0;
}

bool array_iter_has(const array_iter * iter) {
    return iter->bck && iter->idx_in_bck < iter->bck->size;
}

any * array_iter_next_ref(array_iter * iter) {
    any * e = iter->next_ref;
    if (++iter->idx_in_bck == iter->bck->size) {
        iter->bck = iter->bck->next;
        if (iter->bck) {
            iter->next_ref = iter->bck->data;
        }
        iter->idx_in_bck = 0;
    }
    else {
        iter->next_ref += 1;
    }
    return e;
}

any array_iter_next(array_iter * iter) {
    return *array_iter_next_ref(iter);
}
