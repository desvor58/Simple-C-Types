#include <SCT/vecslice.h>

int sct_vecslice_init(sct_vecslice_t *slice, sct_vector_t *src_vec, size_t index, size_t size)
{
    if (!slice) return 1;
    if (!src_vec) return 1;
    if (index >= src_vec->size) return 1;
    if (index + size > src_vec->size) return 1;
    slice->src_vec = src_vec;
    slice->start_index = index;
    slice->_item_size = src_vec->_item_size;
    slice->size = size;
    return 0;
}

void *sct_vecslice_get(sct_vecslice_t *slice, size_t index)
{
    if (!slice) return 0;
    if (index >= slice->size) return 0;
    return slice->src_vec->data + (slice->start_index + index) * slice->_item_size;
}

int sct_vecslice_extend(sct_vecslice_t *slice, size_t size)
{
    if (!slice) return 1;
    if (!size) return 1;
    if (slice->start_index + slice->size + size > slice->src_vec->size) return 1;
    slice->size += size;
    return 0;
}