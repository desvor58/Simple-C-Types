#ifndef SCT_HASHMAP_H
#define SCT_HASHMAP_H

#include "list.h"
#include "vector.h"
#include "arena.h"

#define SCT_HASHMAP_BUCKETS_NUM 128

#define SCT_HASHMAP_CONTAINER_SIZE (sizeof(char*) + map->_item_size)

typedef struct {
    sct_list_t   buckets[SCT_HASHMAP_BUCKETS_NUM];
    sct_arena_t  arena;
    sct_vector_t keys;
    size_t       size;
    size_t       _item_size;
} sct_hashmap_t;

typedef struct {
    const sct_list_t *bucket;
    const sct_list_t *bucket_end;
    u8                *current_pair;
} sct_hashmap_iter_t;

void sct_hashmap_init(sct_hashmap_t *map, size_t item_size);

void sct_hashmap_deinit(sct_hashmap_t *map);

int sct_hashmap_contains(const sct_hashmap_t *map, const char *key);

void sct_hashmap_add(sct_hashmap_t *map, const char *key, const void *item);

void *sct_hashmap_get(const sct_hashmap_t *map, const char *key);

void sct_hashmap_remove(sct_hashmap_t *map, const char *key);

size_t sct_hashmap_size(const sct_hashmap_t *map);

void sct_hashmap_iter_init(sct_hashmap_iter_t *iter, const sct_hashmap_t *map);

int sct_hashmap_iter_next(sct_hashmap_iter_t *iter);

const char *sct_hashmap_iter_key(const sct_hashmap_iter_t *iter);

void *sct_hashmap_iter_value(const sct_hashmap_iter_t *iter);

#endif
