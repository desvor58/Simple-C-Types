#include <SCT/arena_vector.h>

static void arena_vector_size_check(sct_arena_vector_t *vec, size_t size)
{
    size_t required;
    size_t increment;
    size_t new_cap;
    u8 *new_data;

    if (!vec || !vec->arena || !vec->_item_size) {
        abort();
    }
    if (size > SIZE_MAX / vec->_item_size) {
        abort();
    }
    required = size * vec->_item_size;
    if (required <= vec->cap) {
        return;
    }
    if (vec->_item_size > SIZE_MAX / 4) {
        abort();
    }
    increment = vec->_item_size * 4;
    new_cap = vec->cap;
    while (new_cap < required) {
        if (new_cap > SIZE_MAX - increment) {
            new_cap = required;
            break;
        }
        new_cap += increment;
    }
    new_data = sct_arena_alloc(vec->arena, new_cap);
    if (vec->size) {
        memcpy(new_data, vec->data, vec->size * vec->_item_size);
    }
    vec->data = new_data;
    vec->cap = new_cap;
}

void sct_arena_vector_init(sct_arena_vector_t *vec, sct_arena_t *arena, size_t item_size)
{
    vec->arena = arena;
    vec->data = sct_arena_alloc(arena, SCT_ARENA_VECTOR_ALLOC_SIZE);
    vec->cap = SCT_ARENA_VECTOR_ALLOC_SIZE;
    vec->size = 0;
    vec->_item_size = item_size;
}

void sct_arena_vector_deinit(sct_arena_vector_t *vec)
{
    vec->data = NULL;
    vec->size = 0;
    vec->cap = 0;
    vec->_item_size = 0;
    vec->arena = NULL;
}

void sct_arena_vector_push(sct_arena_vector_t *vec, const void *item)
{
    if (vec->size == SIZE_MAX) {
        abort();
    }
    arena_vector_size_check(vec, vec->size + 1);
    memcpy(vec->data + vec->size * vec->_item_size, item, vec->_item_size);
    vec->size++;
}

void sct_arena_vector_push_array(sct_arena_vector_t *vec, const void *arr, size_t size)
{
    if (size > SIZE_MAX - vec->size) {
        abort();
    }
    arena_vector_size_check(vec, vec->size + size);
    if (size) {
        memcpy(vec->data + vec->size * vec->_item_size, arr, vec->_item_size * size);
    }
    vec->size += size;
}

void *sct_arena_vector_get(const sct_arena_vector_t *vec, size_t index)
{
    if (index >= vec->size) {
        return NULL;
    }
    return vec->data + index * vec->_item_size;
}

int sct_arena_vector_set(sct_arena_vector_t *vec, size_t index, const void *item)
{
    if (index >= vec->size) {
        return 1;
    }
    memcpy(vec->data + index * vec->_item_size, item, vec->_item_size);
    return 0;
}

void *sct_arena_vector_pop(sct_arena_vector_t *vec)
{
    if (!vec->size) {
        return NULL;
    }
    vec->size--;
    return vec->data + vec->size * vec->_item_size;
}

void sct_arena_vector_erase(sct_arena_vector_t *vec, size_t index)
{
    size_t tail_size;
    if (!vec->size || index >= vec->size) {
        return;
    }
    tail_size = vec->size - index - 1;
    if (tail_size) {
        memmove(vec->data + index * vec->_item_size,
                vec->data + (index + 1) * vec->_item_size,
                tail_size * vec->_item_size);
    }
    vec->size--;
}

int sct_arena_vector_swap_remove(sct_arena_vector_t *vec, size_t index)
{
    if (index >= vec->size) {
        return 1;
    }
    if (index + 1 < vec->size) {
        memcpy(vec->data + index * vec->_item_size,
               vec->data + (vec->size - 1) * vec->_item_size,
               vec->_item_size);
    }
    vec->size--;
    return 0;
}

void sct_arena_vector_insert(sct_arena_vector_t *vec, size_t index, const void *item)
{
    if (!vec || index > vec->size) {
        return;
    }
    if (index == vec->size) {
        sct_arena_vector_push(vec, item);
        return;
    }
    if (vec->size == SIZE_MAX) {
        abort();
    }
    arena_vector_size_check(vec, vec->size + 1);
    memmove(vec->data + (index + 1) * vec->_item_size,
            vec->data + index * vec->_item_size,
            (vec->size - index) * vec->_item_size);
    memcpy(vec->data + index * vec->_item_size, item, vec->_item_size);
    vec->size++;
}

int sct_arena_vector_reserve(sct_arena_vector_t *vec, size_t capacity)
{
    size_t required;
    size_t increment;
    size_t new_cap;
    u8 *new_data;

    if (!vec || !vec->arena ||
        (vec->_item_size && capacity > SIZE_MAX / vec->_item_size)) {
        return 1;
    }
    if (!capacity || !vec->_item_size) {
        return 0;
    }
    required = capacity * vec->_item_size;
    if (required <= vec->cap) {
        return 0;
    }
    if (vec->_item_size > SIZE_MAX / 4) {
        abort();
    }
    increment = vec->_item_size * 4;
    new_cap = vec->cap;
    while (new_cap < required) {
        if (new_cap > SIZE_MAX - increment) {
            new_cap = required;
            break;
        }
        new_cap += increment;
    }
    new_data = sct_arena_alloc(vec->arena, new_cap);
    if (vec->size) {
        memcpy(new_data, vec->data, vec->size * vec->_item_size);
    }
    vec->data = new_data;
    vec->cap = new_cap;
    return 0;
}

size_t sct_arena_vector_size(const sct_arena_vector_t *vec)
{
    return vec ? vec->size : 0;
}

size_t sct_arena_vector_capacity(const sct_arena_vector_t *vec)
{
    return vec && vec->_item_size ? vec->cap / vec->_item_size : 0;
}
