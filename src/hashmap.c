#include <SCT/hashmap.h>

static u32 hash_fnv1a(const void *key, size_t len)
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

static inline char *container_get_key(const u8 *container)
{
    return *(char**)container;
}

static inline void container_set_key(u8 *container, const char *key)
{
    *(const char**)container = key;
}

static inline void *container_get_val(const u8 *container)
{
    return (u8*)container + sizeof(char*);
}

static u8 *hashmap_find_pair(const sct_hashmap_t *map, const char *key, u32 hash)
{
    const sct_list_t *bucket = &map->buckets[hash % SCT_HASHMAP_BUCKETS_NUM];
    u8 *pair = bucket->first_pair ? *(void**)bucket->first_pair : NULL;
    while (pair) {
        const u8 *container = pair + sizeof(void*);
        if (!strcmp(container_get_key(container), key)) {
            return pair;
        }
        pair = *(void**)pair;
    }
    return NULL;
}

void sct_hashmap_init(sct_hashmap_t *map, size_t item_size)
{
    size_t i;
    if (item_size > SIZE_MAX - sizeof(void*) - sizeof(char*)) {
        abort();
    }
    map->_item_size = item_size;
    for (i = 0; i < SCT_HASHMAP_BUCKETS_NUM; i++) {
        sct_list_init(&map->buckets[i], SCT_HASHMAP_CONTAINER_SIZE);
    }
    sct_arena_init(&map->arena);
    sct_vector_init(&map->keys, sizeof(char*));
    map->size = 0;
}

void sct_hashmap_deinit(sct_hashmap_t *map)
{
    size_t i;
    for (i = 0; i < SCT_HASHMAP_BUCKETS_NUM; i++) {
        sct_list_deinit(&map->buckets[i]);
    }
    sct_arena_deinit(&map->arena);
    sct_vector_deinit(&map->keys);
    map->size = 0;
    map->_item_size = 0;
}

int sct_hashmap_contains(const sct_hashmap_t *map, const char *key)
{
    return hashmap_find_pair(map, key, hash_fnv1a(key, strlen(key))) != NULL;
}

void sct_hashmap_add(sct_hashmap_t *map, const char *key, const void *item)
{
    size_t key_len = strlen(key);
    u32 hash = hash_fnv1a(key, key_len);
    u8 *pair = hashmap_find_pair(map, key, hash);
    sct_list_t *bucket;
    u8 *container;
    char *key_copy;

    if (pair) {
        if (map->_item_size) {
            memcpy(container_get_val(pair + sizeof(void*)), item, map->_item_size);
        }
        return;
    }
    if (key_len == SIZE_MAX) {
        abort();
    }
    bucket = &map->buckets[hash % SCT_HASHMAP_BUCKETS_NUM];
    container = sct_arena_alloc(&map->arena, SCT_HASHMAP_CONTAINER_SIZE);
    key_copy = sct_arena_alloc(&map->arena, key_len + 1);
    memcpy(key_copy, key, key_len + 1);
    container_set_key(container, key_copy);
    if (map->_item_size) {
        memcpy(container_get_val(container), item, map->_item_size);
    }
    sct_list_push(bucket, container);
    sct_vector_push(&map->keys, &key_copy);
    map->size++;
}

void *sct_hashmap_get(const sct_hashmap_t *map, const char *key)
{
    u8 *pair = hashmap_find_pair(map, key, hash_fnv1a(key, strlen(key)));
    return pair ? container_get_val(pair + sizeof(void*)) : NULL;
}

void sct_hashmap_remove(sct_hashmap_t *map, const char *key)
{
    size_t key_len = strlen(key);
    u32 hash = hash_fnv1a(key, key_len);
    u8 *pair = hashmap_find_pair(map, key, hash);
    sct_list_iter_t iter;
    size_t i;
    int removed = 0;
    if (!pair) {
        return;
    }
    sct_list_iter_init(&iter, &map->buckets[hash % SCT_HASHMAP_BUCKETS_NUM]);
    while (sct_list_iter_next(&iter)) {
        if (iter.current == pair) {
            sct_list_iter_erase(&iter);
            removed = 1;
            break;
        }
    }
    if (!removed) {
        return;
    }
    for (i = 0; i < map->keys.size; i++) {
        char **stored_key = sct_vector_get(&map->keys, i);
        if (*stored_key == (char *)key || !strcmp(*stored_key, key)) {
            sct_vector_erase(&map->keys, i);
            break;
        }
    }
    map->size--;
}

size_t sct_hashmap_size(const sct_hashmap_t *map)
{
    return map ? map->size : 0;
}

void sct_hashmap_iter_init(sct_hashmap_iter_t *iter, const sct_hashmap_t *map)
{
    iter->bucket = map->buckets;
    iter->bucket_end = map->buckets + SCT_HASHMAP_BUCKETS_NUM;
    iter->current_pair = NULL;
}

int sct_hashmap_iter_next(sct_hashmap_iter_t *iter)
{
    if (iter->current_pair) {
        iter->current_pair = *(void**)iter->current_pair;
    }
    while (!iter->current_pair && iter->bucket != iter->bucket_end) {
        if (iter->bucket->first_pair) {
            iter->current_pair = *(void**)iter->bucket->first_pair;
        }
        iter->bucket++;
    }
    return iter->current_pair != NULL;
}

const char *sct_hashmap_iter_key(const sct_hashmap_iter_t *iter)
{
    return iter->current_pair
        ? container_get_key(iter->current_pair + sizeof(void*))
        : NULL;
}

void *sct_hashmap_iter_value(const sct_hashmap_iter_t *iter)
{
    return iter->current_pair
        ? container_get_val(iter->current_pair + sizeof(void*))
        : NULL;
}
