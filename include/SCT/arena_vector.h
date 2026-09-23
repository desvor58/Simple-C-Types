#ifndef SCT_ARENA_VECTOR_H
#define SCT_ARENA_VECTOR_H

#include "common.h"
#include "arena.h"

#define SCT_ARENA_VECTOR_ALLOC_SIZE 128

typedef struct {
    u8           *data;
    size_t        size;
    size_t        cap;
    size_t        _item_size;
    sct_arena_t  *arena;
} sct_arena_vector_t;

void sct_arena_vector_init(sct_arena_vector_t *vec, sct_arena_t *arena, size_t item_size);

void sct_arena_vector_deinit(sct_arena_vector_t *vec);

void sct_arena_vector_push(sct_arena_vector_t *vec, const void *item);

void sct_arena_vector_push_array(sct_arena_vector_t *vec, const void *arr, size_t size);

void *sct_arena_vector_get(const sct_arena_vector_t *vec, size_t index);

int sct_arena_vector_set(sct_arena_vector_t *vec, size_t index, const void *item);

void *sct_arena_vector_pop(sct_arena_vector_t *vec);

void sct_arena_vector_erase(sct_arena_vector_t *vec, size_t index);

int sct_arena_vector_swap_remove(sct_arena_vector_t *vec, size_t index);

void sct_arena_vector_insert(sct_arena_vector_t *vec, size_t index, const void *item);

int sct_arena_vector_reserve(sct_arena_vector_t *vec, size_t capacity);

size_t sct_arena_vector_size(const sct_arena_vector_t *vec);

size_t sct_arena_vector_capacity(const sct_arena_vector_t *vec);

#endif
