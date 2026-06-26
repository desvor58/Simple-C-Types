#include <SCT/vector.h>

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
}

void sct_vector_push(sct_vector_t *vec, void *item)
{
    if ((vec->size + 1) * vec->_item_size > vec->cap) {
        vec->cap += (vec->_item_size / SCT_VECTOR_ALLOC_SIZE + 1) * SCT_VECTOR_ALLOC_SIZE;
        u8 *new_data = amalloc(vec->cap);
        if (vec->size > 0) {
            memcpy(new_data, vec->data, vec->size * vec->_item_size);
        }
        free(vec->data);
        vec->data = new_data;
    }
    memcpy(vec->data + vec->size * vec->_item_size, item, vec->_item_size);
    vec->size++;
}

void *sct_vector_get(sct_vector_t *vec, size_t index)
{
    return vec->data + index * vec->_item_size;
}

void *sct_vector_pop(sct_vector_t *vec)
{
    return vec->data + --vec->size * vec->_item_size;
}

void sct_vector_erase(sct_vector_t *vec, size_t index)
{
    if (!vec->size || index >= vec->size) return;
    if (index < vec->size - 1) {
        memmove(vec->data + index * vec->_item_size,
                vec->data + (index + 1) * vec->_item_size,
                (vec->size - index - 1) * vec->_item_size);
    }
    vec->size--;
}
