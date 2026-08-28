#include <SCT/hashmap.h>

static u32 hash_fnv1a(const void *key, size_t len)
{
    u32 hash = 2166136261U;
    const u8 *data = (const u8*)key;
    for (size_t i = 0; i < len; i++) {
        hash ^= data[i];
        hash *= 16777619U;
    }
    return hash;
}

static inline char *container_get_key(u8 *container)
{
    return *(char**)container;
}

static inline void container_set_key(u8 *container, const char *key)
{
    *(const char**)container = key;
}

static inline void *container_get_val(u8 *container)
{
    return container + sizeof(char*);
}

void sct_hashmap_init(sct_hashmap_t *map, size_t item_size)
{
    map->_item_size = item_size;
    for (size_t i = 0; i < SCT_HASHMAP_BUCKETS_NUM; i++) {
        sct_list_init(&map->buckets[i], SCT_HASHMAP_CONTAINER_SIZE);
    }
    sct_arena_init(&map->arena);
    sct_vector_init(&map->keys, sizeof(char*));
}

void sct_hashmap_deinit(sct_hashmap_t *map)
{
    for (size_t i = 0; i < SCT_HASHMAP_BUCKETS_NUM; i++) {
        sct_list_deinit(&map->buckets[i]);
    }
    sct_arena_deinit(&map->arena);
    sct_vector_deinit(&map->keys);
}

int sct_hashmap_contains(sct_hashmap_t *map, const char *key)
{
    sct_list_t *bkt = &map->buckets[hash_fnv1a(key, strlen(key)) % SCT_HASHMAP_BUCKETS_NUM];
    
    foreach(bkt) {
        u8 *container = (u8*)cur_pair + sizeof(void*);
        if (!strcmp(container_get_key(container), key)) {
            return 1;
        }
    }
    return 0;
}

void sct_hashmap_add(sct_hashmap_t *map, const char *key, void *item)
{
    size_t key_len = strlen(key);
    sct_list_t *bkt = &map->buckets[hash_fnv1a(key, key_len) % SCT_HASHMAP_BUCKETS_NUM];
    if (sct_hashmap_contains(map, key)) {
        foreach (bkt) {
            u8 *container = (u8*)cur_pair + sizeof(void*);
            if (!strcmp(container_get_key(container), key)) {
                memcpy(container_get_val(container), item, map->_item_size);
                return;
            }
        }
        return;
    }
    u8 *container = sct_arena_alloc(&map->arena, SCT_HASHMAP_CONTAINER_SIZE);
    char *key_copy = sct_arena_alloc(&map->arena, key_len + 1);
    strcpy(key_copy, key);
    container_set_key(container, key_copy);
    memcpy(container_get_val(container), item, map->_item_size);
    sct_list_push(bkt, container);
    sct_vector_push(&map->keys, &key_copy);
}

void *sct_hashmap_get(sct_hashmap_t *map, const char *key)
{
    size_t key_len = strlen(key);
    sct_list_t *bkt = &map->buckets[hash_fnv1a(key, key_len) % SCT_HASHMAP_BUCKETS_NUM];

    foreach (bkt) {
        u8 *container = (u8*)cur_pair + sizeof(void*);
        if (!strcmp(container_get_key(container), key)) {
            return container_get_val(container);
        }
    }
    return 0;
}

void sct_hashmap_remove(sct_hashmap_t *map, const char *key)
{
    size_t key_len = strlen(key);
    sct_list_t *bkt = &map->buckets[hash_fnv1a(key, key_len) % SCT_HASHMAP_BUCKETS_NUM];

    size_t i = 0;
    foreach (bkt) {
        u8 *container = (u8*)cur_pair + sizeof(void*);
        if (!strcmp(container_get_key(container), key)) {
            sct_list_erase(bkt, i);
            break;
        }
        i++;
    }
    for (size_t i = 0; i < map->keys.size; i++) {
        char **k = sct_vector_get(&map->keys, i);
        if (!k) return;
        if (!strcmp(*k, key)) {
            sct_vector_erase(&map->keys, i);
            break;
        }
    }
}
