#include <SCT/vector.h>

static void vector_size_check(sct_vector_t *vec, size_t size)
{
    size_t required;
    size_t increment;
    size_t new_cap;

    if (!vec || !vec->_item_size) {
        if (!vec || size) {
            abort();
        }
        return;
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
    vec->data = realloc(vec->data, new_cap);
    if (!vec->data) {
        fprintf(stderr, "vector reallocation to %zu bytes failed\n", new_cap);
        abort();
    }
    vec->cap = new_cap;
}

void sct_vector_init(sct_vector_t *vec, size_t item_size)
{
    vec->data = amalloc(SCT_VECTOR_ALLOC_SIZE);
    vec->cap = SCT_VECTOR_ALLOC_SIZE;
    vec->size = 0;
    vec->_item_size = item_size;
}

void sct_vector_deinit(sct_vector_t *vec)
{
    free(vec->data);
    vec->data = NULL;
    vec->size = 0;
    vec->cap = 0;
    vec->_item_size = 0;
}

void sct_vector_push(sct_vector_t *vec, const void *item)
{
    if (vec->size == SIZE_MAX) {
        abort();
    }
    vector_size_check(vec, vec->size + 1);
    memcpy(vec->data + vec->size * vec->_item_size, item, vec->_item_size);
    vec->size++;
}

void sct_vector_push_array(sct_vector_t *vec, const void *arr, size_t size)
{
    if (size > SIZE_MAX - vec->size) {
        abort();
    }
    vector_size_check(vec, vec->size + size);
    if (size && vec->_item_size) {
        memcpy(vec->data + vec->size * vec->_item_size, arr, vec->_item_size * size);
    }
    vec->size += size;
}

void *sct_vector_get(const sct_vector_t *vec, size_t index)
{
    if (index >= vec->size) {
        return NULL;
    }
    return vec->data + index * vec->_item_size;
}

int sct_vector_set(sct_vector_t *vec, size_t index, const void *item)
{
    if (index >= vec->size) {
        return 1;
    }
    memcpy(vec->data + index * vec->_item_size, item, vec->_item_size);
    return 0;
}

void *sct_vector_pop(sct_vector_t *vec)
{
    if (!vec->size) {
        return NULL;
    }
    vec->size--;
    return vec->data + vec->size * vec->_item_size;
}

void sct_vector_erase(sct_vector_t *vec, size_t index)
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

int sct_vector_swap_remove(sct_vector_t *vec, size_t index)
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

void sct_vector_insert(sct_vector_t *vec, size_t index, const void *item)
{
    if (!vec || index > vec->size) {
        return;
    }
    if (index == vec->size) {
        sct_vector_push(vec, item);
        return;
    }
    if (vec->size == SIZE_MAX) {
        abort();
    }
    vector_size_check(vec, vec->size + 1);
    memmove(vec->data + (index + 1) * vec->_item_size,
            vec->data + index * vec->_item_size,
            (vec->size - index) * vec->_item_size);
    memcpy(vec->data + index * vec->_item_size, item, vec->_item_size);
    vec->size++;
}

int sct_vector_reserve(sct_vector_t *vec, size_t capacity)
{
    size_t required;
    size_t increment;
    size_t new_cap;
    u8 *new_data;

    if (!vec || (vec->_item_size && capacity > SIZE_MAX / vec->_item_size)) {
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
    new_data = realloc(vec->data, new_cap);
    if (!new_data) {
        fprintf(stderr, "vector reserve to %zu bytes failed\n", new_cap);
        abort();
    }
    vec->data = new_data;
    vec->cap = new_cap;
    return 0;
}

size_t sct_vector_size(const sct_vector_t *vec)
{
    return vec ? vec->size : 0;
}

size_t sct_vector_capacity(const sct_vector_t *vec)
{
    return vec && vec->_item_size ? vec->cap / vec->_item_size : 0;
}
