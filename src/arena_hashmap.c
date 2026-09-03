#include <SCT/arena_hashmap.h>

#define SCT_ARENA_HASHMAP_CONTAINER_SIZE sizeof(char*) + map->_item_size

static u32 ahash_fnv1a(const void *key, size_t len)
{
    u32 hash = 2166136261U;
    const u8 *data = (const u8*)key;
    for (size_t i = 0; i < len; i++) {
        hash ^= data[i];
        hash *= 16777619U;
    }
    return hash;
}

static inline char *acontainer_get_key(u8 *container)
{
    return *(char**)container;
}

static inline void acontainer_set_key(u8 *container, const char *key)
{
    *(const char**)container = key;
}

static inline void *acontainer_get_val(u8 *container)
{
    return container + sizeof(char*);
}

void sct_arena_hashmap_init(sct_arena_hashmap_t *map, sct_arena_t *arena, size_t item_size)
{
    map->arena = arena;
    map->_item_size = item_size;
    for (size_t i = 0; i < SCT_ARENA_HASHMAP_BUCKETS_NUM; i++) {
        sct_arena_list_init(&map->buckets[i], arena, SCT_ARENA_HASHMAP_CONTAINER_SIZE);
    }
    sct_arena_vector_init(&map->keys, arena, sizeof(char*));
}

void sct_arena_hashmap_deinit(sct_arena_hashmap_t *map)
{
}

int sct_arena_hashmap_contains(sct_arena_hashmap_t *map, const char *key)
{
    sct_arena_list_t *bkt = &map->buckets[ahash_fnv1a(key, strlen(key)) % SCT_ARENA_HASHMAP_BUCKETS_NUM];

    sct_arena_foreach(bkt) {
        u8 *container = (u8*)cur_pair + sizeof(void*);
        if (!strcmp(acontainer_get_key(container), key)) {
            return 1;
        }
    }
    return 0;
}

void sct_arena_hashmap_add(sct_arena_hashmap_t *map, const char *key, void *item)
{
    size_t key_len = strlen(key);
    sct_arena_list_t *bkt = &map->buckets[ahash_fnv1a(key, key_len) % SCT_ARENA_HASHMAP_BUCKETS_NUM];
    if (sct_arena_hashmap_contains(map, key)) {
        sct_arena_foreach (bkt) {
            u8 *container = (u8*)cur_pair + sizeof(void*);
            if (!strcmp(acontainer_get_key(container), key)) {
                memcpy(acontainer_get_val(container), item, map->_item_size);
                return;
            }
        }
        return;
    }
    u8 *container = sct_arena_alloc(map->arena, SCT_ARENA_HASHMAP_CONTAINER_SIZE);
    char *key_copy = sct_arena_alloc(map->arena, key_len + 1);
    strcpy(key_copy, key);
    acontainer_set_key(container, key_copy);
    memcpy(acontainer_get_val(container), item, map->_item_size);
    sct_arena_list_push(bkt, container);
    sct_arena_vector_push(&map->keys, &key_copy);
}

void *sct_arena_hashmap_get(sct_arena_hashmap_t *map, const char *key)
{
    size_t key_len = strlen(key);
    sct_arena_list_t *bkt = &map->buckets[ahash_fnv1a(key, key_len) % SCT_ARENA_HASHMAP_BUCKETS_NUM];

    sct_arena_foreach (bkt) {
        u8 *container = (u8*)cur_pair + sizeof(void*);
        if (!strcmp(acontainer_get_key(container), key)) {
            return acontainer_get_val(container);
        }
    }
    return 0;
}

void sct_arena_hashmap_remove(sct_arena_hashmap_t *map, const char *key)
{
    size_t key_len = strlen(key);
    sct_arena_list_t *bkt = &map->buckets[ahash_fnv1a(key, key_len) % SCT_ARENA_HASHMAP_BUCKETS_NUM];

    size_t i = 0;
    sct_arena_foreach (bkt) {
        u8 *container = (u8*)cur_pair + sizeof(void*);
        if (!strcmp(acontainer_get_key(container), key)) {
            sct_arena_list_erase(bkt, i);
            break;
        }
        i++;
    }
    for (size_t i = 0; i < map->keys.size; i++) {
        char **k = sct_arena_vector_get(&map->keys, i);
        if (!k) return;
        if (!strcmp(*k, key)) {
            sct_arena_vector_erase(&map->keys, i);
            break;
        }
    }
}