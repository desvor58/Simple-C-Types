#ifndef SCT_ARENA_HASHMAP_H
#define SCT_ARENA_HASHMAP_H

#include "arena_list.h"

#define SCT_ARENA_HASHMAP_BUCKETS_NUM 128

typedef struct {
    sct_arena_list_t *buckets;
    sct_arena_t      *arena;
    size_t            bucket_count;
    size_t            size;
    size_t            _item_size;
} sct_arena_hashmap_t;

void sct_arena_hashmap_init(sct_arena_hashmap_t *map, sct_arena_t *arena, size_t item_size);

void sct_arena_hashmap_deinit(sct_arena_hashmap_t *map);

int sct_arena_hashmap_contains(const sct_arena_hashmap_t *map, const char *key);

void sct_arena_hashmap_add(sct_arena_hashmap_t *map, const char *key, const void *item);

void *sct_arena_hashmap_get(const sct_arena_hashmap_t *map, const char *key);

void sct_arena_hashmap_remove(sct_arena_hashmap_t *map, const char *key);

size_t sct_arena_hashmap_size(const sct_arena_hashmap_t *map);

#endif
