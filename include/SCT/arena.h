#ifndef SCT_ARENA_H
#define SCT_ARENA_H

#include "common.h"

#define SCT_ARENA_ALLOC_SIZE 2 * 1024
#define SCT_ARENA_FREE_NONALLOC_LIMIT (SCT_ARENA_ALLOC_SIZE * 2)
#define SCT_ARENA_DEFAULT_ALIGNMENT 16

typedef struct sct_arena_block sct_arena_block_t;

typedef struct {
    u8                *arena;
    size_t             size;
    size_t             cap;
    size_t             alloc_size;
    sct_arena_block_t *blocks;
    sct_arena_block_t *cur;
} sct_arena_t;

void sct_arena_init(sct_arena_t *arena);

void sct_arena_deinit(sct_arena_t *arena);

void *sct_arena_alloc(sct_arena_t *arena, size_t size);

void *sct_arena_alloc_zero(sct_arena_t *arena, size_t size);

void *sct_arena_alloc_aligned(sct_arena_t *arena, size_t size, size_t alignment);

void *sct_arena_realloc(sct_arena_t *arena, void *ptr, size_t old_size, size_t new_size);

void sct_arena_free(sct_arena_t *arena);

#endif
