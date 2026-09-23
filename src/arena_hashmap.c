#include <SCT/arena_hashmap.h>

static u32 ahash_fnv1a(const void *key, size_t len)
{
    u32 hash = 2166136261U;
    const u8 *data = (const u8*)key;
    size_t i;
    for (i = 0; i < len; i++) {
        hash ^= data[i];
        hash *= 16777619U;
    }
    return hash;
}

static inline char *acontainer_get_key(const u8 *container)
{
    return *(char**)container;
}

static inline void acontainer_set_key(u8 *container, const char *key)
{
    *(const char**)container = key;
}

static inline void *acontainer_get_val(const u8 *container)
{
    return (u8*)container + sizeof(char*);
}

static u8 *ahashmap_find_pair(const sct_arena_hashmap_t *map, const char *key, u32 hash)
{
    const sct_arena_list_t *bucket = &map->buckets[hash % map->bucket_count];
    u8 *pair = bucket->first_pair ? *(void**)bucket->first_pair : NULL;
    while (pair) {
        const u8 *container = pair + sizeof(void*);
        if (!strcmp(acontainer_get_key(container), key)) {
            return pair;
        }
        pair = *(void**)pair;
    }
    return NULL;
}

static void ahashmap_append_pair(sct_arena_hashmap_t *map, sct_arena_list_t *bucket, u8 *pair)
{
    if (!bucket->first_pair) {
        bucket->first_pair = sct_arena_alloc(map->arena,
            sizeof(void*) + sizeof(char*) + map->_item_size);
        *(void**)bucket->first_pair = NULL;
        bucket->last_pair = bucket->first_pair;
    }
    *(void**)pair = NULL;
    *(void**)bucket->last_pair = pair;
    bucket->last_pair = pair;
    bucket->size++;
}

static void ahashmap_resize(sct_arena_hashmap_t *map, size_t new_count)
{
    sct_arena_list_t *old_buckets = map->buckets;
    size_t old_count = map->bucket_count;
    sct_arena_list_t *new_buckets;
    size_t i;

    if (!new_count || new_count > SIZE_MAX / sizeof(sct_arena_list_t)) {
        abort();
    }
    new_buckets = sct_arena_alloc(map->arena, new_count * sizeof(sct_arena_list_t));
    for (i = 0; i < new_count; i++) {
        sct_arena_list_init(&new_buckets[i], map->arena, sizeof(char*) + map->_item_size);
    }
    for (i = 0; i < old_count; i++) {
        u8 *pair = old_buckets[i].first_pair
            ? *(void**)old_buckets[i].first_pair
            : NULL;
        old_buckets[i].first_pair = NULL;
        old_buckets[i].last_pair = NULL;
        old_buckets[i].size = 0;
        while (pair) {
            u8 *next = *(void**)pair;
            const char *key = acontainer_get_key(pair + sizeof(void*));
            u32 hash = ahash_fnv1a(key, strlen(key));
            sct_arena_list_t *bucket = &new_buckets[hash % new_count];
            ahashmap_append_pair(map, bucket, pair);
            pair = next;
        }
    }
    map->buckets = new_buckets;
    map->bucket_count = new_count;
}

static void ahashmap_ensure_capacity(sct_arena_hashmap_t *map)
{
    size_t threshold = map->bucket_count - map->bucket_count / 4;
    if (map->size < threshold) {
        return;
    }
    if (map->bucket_count > SIZE_MAX / 2) {
        abort();
    }
    ahashmap_resize(map, map->bucket_count * 2);
}

void sct_arena_hashmap_init(sct_arena_hashmap_t *map, sct_arena_t *arena, size_t item_size)
{
    size_t i;
    if (item_size > SIZE_MAX - sizeof(void*) - sizeof(char*)) {
        abort();
    }
    if (SCT_ARENA_HASHMAP_BUCKETS_NUM > SIZE_MAX / sizeof(sct_arena_list_t)) {
        abort();
    }
    map->arena = arena;
    map->_item_size = item_size;
    map->bucket_count = SCT_ARENA_HASHMAP_BUCKETS_NUM;
    map->size = 0;
    map->buckets = sct_arena_alloc(arena, map->bucket_count * sizeof(sct_arena_list_t));
    for (i = 0; i < map->bucket_count; i++) {
        sct_arena_list_init(&map->buckets[i], arena, sizeof(char*) + item_size);
    }
}

void sct_arena_hashmap_deinit(sct_arena_hashmap_t *map)
{
    map->buckets = NULL;
    map->arena = NULL;
    map->bucket_count = 0;
    map->size = 0;
    map->_item_size = 0;
}

int sct_arena_hashmap_contains(const sct_arena_hashmap_t *map, const char *key)
{
    return ahashmap_find_pair(map, key, ahash_fnv1a(key, strlen(key))) != NULL;
}

void sct_arena_hashmap_add(sct_arena_hashmap_t *map, const char *key, const void *item)
{
    size_t key_len = strlen(key);
    u32 hash = ahash_fnv1a(key, key_len);
    u8 *pair = ahashmap_find_pair(map, key, hash);
    sct_arena_list_t *bucket;
    u8 *container;
    char *key_copy;

    if (pair) {
        if (map->_item_size) {
            memcpy(acontainer_get_val(pair + sizeof(void*)), item, map->_item_size);
        }
        return;
    }
    if (key_len == SIZE_MAX) {
        abort();
    }
    ahashmap_ensure_capacity(map);
    bucket = &map->buckets[hash % map->bucket_count];
    container = sct_arena_alloc(map->arena, sizeof(char*) + map->_item_size);
    key_copy = sct_arena_alloc(map->arena, key_len + 1);
    memcpy(key_copy, key, key_len + 1);
    acontainer_set_key(container, key_copy);
    if (map->_item_size) {
        memcpy(acontainer_get_val(container), item, map->_item_size);
    }
    sct_arena_list_push(bucket, container);
    map->size++;
}

void *sct_arena_hashmap_get(const sct_arena_hashmap_t *map, const char *key)
{
    u8 *pair = ahashmap_find_pair(map, key, ahash_fnv1a(key, strlen(key)));
    return pair ? acontainer_get_val(pair + sizeof(void*)) : NULL;
}

void sct_arena_hashmap_remove(sct_arena_hashmap_t *map, const char *key)
{
    u32 hash = ahash_fnv1a(key, strlen(key));
    u8 *pair = ahashmap_find_pair(map, key, hash);
    sct_arena_list_iter_t iter;
    if (!pair) {
        return;
    }
    sct_arena_list_iter_init(&iter, &map->buckets[hash % map->bucket_count]);
    while (sct_arena_list_iter_next(&iter)) {
        if (iter.current == pair) {
            sct_arena_list_iter_erase(&iter);
            map->size--;
            return;
        }
    }
}

size_t sct_arena_hashmap_size(const sct_arena_hashmap_t *map)
{
    return map ? map->size : 0;
}
