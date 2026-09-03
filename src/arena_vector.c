#include <SCT/arena_vector.h>

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
}

static void arena_vector_size_check(sct_arena_vector_t *vec, size_t size)
{
    if ((vec->size + size) * vec->_item_size > vec->cap) {
        vec->cap += (vec->_item_size * size / SCT_ARENA_VECTOR_ALLOC_SIZE + 1) * SCT_ARENA_VECTOR_ALLOC_SIZE;
        u8 *new_data = sct_arena_alloc(vec->arena, vec->cap);
        if (vec->size > 0) {
            memcpy(new_data, vec->data, vec->size * vec->_item_size);
        }
        vec->data = new_data;
    }
}

void sct_arena_vector_push(sct_arena_vector_t *vec, void *item)
{
    arena_vector_size_check(vec, 1);
    memcpy(vec->data + vec->size * vec->_item_size, item, vec->_item_size);
    vec->size++;
}

void sct_arena_vector_push_array(sct_arena_vector_t *vec, void *arr, size_t size)
{
    arena_vector_size_check(vec, size);
    memcpy(vec->data + vec->size * vec->_item_size, arr, vec->_item_size * size);
    vec->size += size;
}

void *sct_arena_vector_get(sct_arena_vector_t *vec, size_t index)
{
    if (index >= vec->size) return 0;
    return vec->data + index * vec->_item_size;
}

int sct_arena_vector_set(sct_arena_vector_t *vec, size_t index, void *item)
{
    if (index >= vec->size) return 1;
    memcpy(vec->data + index * vec->_item_size, item, vec->_item_size);
    return 0;
}

void *sct_arena_vector_pop(sct_arena_vector_t *vec)
{
    return vec->data + --vec->size * vec->_item_size;
}

void sct_arena_vector_erase(sct_arena_vector_t *vec, size_t index)
{
    if (!vec->size || index >= vec->size) return;
    if (index < vec->size - 1) {
        memmove(vec->data + index * vec->_item_size,
                vec->data + (index + 1) * vec->_item_size,
                (vec->size - index - 1) * vec->_item_size);
    }
    vec->size--;
}

void sct_arena_vector_insert(sct_arena_vector_t *vec, size_t index, void *item)
{
    if (!vec || index > vec->size) return;
    if (index == vec->size) {
        sct_arena_vector_push(vec, item);
        return;
    }
    arena_vector_size_check(vec, 1);

    u8 *insert_pos = vec->data + (index * vec->_item_size);
    size_t slice_size = (vec->size - index) * vec->_item_size;

    memmove(insert_pos + vec->_item_size, insert_pos, slice_size);
    memcpy(insert_pos, item, vec->_item_size);

    vec->size++;
}