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

static void vector_size_check(sct_vector_t *vec, size_t size)
{
    if ((vec->size + size) * vec->_item_size > vec->cap) {
        vec->cap += (vec->_item_size * size / SCT_VECTOR_ALLOC_SIZE + 1) * SCT_VECTOR_ALLOC_SIZE;
        u8 *new_data = amalloc(vec->cap);
        if (vec->size > 0) {
            memcpy(new_data, vec->data, vec->size * vec->_item_size);
        }
        free(vec->data);
        vec->data = new_data;
    }
}

void sct_vector_push(sct_vector_t *vec, void *item)
{
    vector_size_check(vec, 1);
    memcpy(vec->data + vec->size * vec->_item_size, item, vec->_item_size);
    vec->size++;
}

void sct_vector_push_array(sct_vector_t *vec, void *arr, size_t size)
{
    vector_size_check(vec, size);
    memcpy(vec->data + vec->size * vec->_item_size, arr, vec->_item_size * size);
    vec->size += size;
}

void *sct_vector_get(sct_vector_t *vec, size_t index)
{
    if (index >= vec->size) return 0;
    return vec->data + index * vec->_item_size;
}

int sct_vector_set(sct_vector_t *vec, size_t index, void *item)
{
    if (index >= vec->size) return 1;
    memcpy(vec->data + index * vec->_item_size, item, vec->_item_size);
    return 0;
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

void sct_vector_insert(sct_vector_t *vec, size_t index, void *item)
{
    if (!vec || index > vec->size) return;
    if (index == vec->size) {
        sct_vector_push(vec, item);
        return;
    }
    vector_size_check(vec, 1);

    u8 *insert_pos = vec->data + (index * vec->_item_size);
    size_t slice_size = (vec->size - index) * vec->_item_size;

    memmove(insert_pos + vec->_item_size, insert_pos, slice_size);
    memcpy(insert_pos, item, vec->_item_size);
    
    vec->size++;
}
