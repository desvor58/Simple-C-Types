#include <SCT/vecslice.h>

int sct_vecslice_init(sct_vecslice_t *slice, const sct_vector_t *src_vec, size_t index, size_t size)
{
    if (!slice || !src_vec || index > src_vec->size) {
        return 1;
    }
    if (size > src_vec->size - index) {
        return 1;
    }
    slice->src_vec = src_vec;
    slice->start_index = index;
    slice->_item_size = src_vec->_item_size;
    slice->size = size;
    return 0;
}

void *sct_vecslice_get(const sct_vecslice_t *slice, size_t index)
{
    if (!slice || !slice->src_vec || index >= slice->size ||
        slice->start_index > slice->src_vec->size ||
        index >= slice->src_vec->size - slice->start_index) {
        return NULL;
    }
    return slice->src_vec->data + (slice->start_index + index) * slice->_item_size;
}

int sct_vecslice_extend(sct_vecslice_t *slice, size_t size)
{
    size_t start;
    if (!slice || !slice->src_vec || !size || slice->start_index > slice->src_vec->size) {
        return 1;
    }
    if (slice->size > slice->src_vec->size - slice->start_index) {
        return 1;
    }
    start = slice->start_index + slice->size;
    if (start > slice->src_vec->size || size > slice->src_vec->size - start) {
        return 1;
    }
    slice->size += size;
    return 0;
}

int sct_vecslice_advance(sct_vecslice_t *slice, size_t size)
{
    if (!slice || !slice->src_vec || !size || slice->start_index > slice->src_vec->size) {
        return 1;
    }
    if (size > slice->src_vec->size - slice->start_index) {
        return 1;
    }
    slice->start_index += size;
    slice->size = size < slice->size ? slice->size - size : 0;
    return 0;
}

size_t sct_vecslice_size(const sct_vecslice_t *slice)
{
    return slice ? slice->size : 0;
}
