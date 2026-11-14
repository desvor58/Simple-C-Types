#ifndef SCT_VECSLICE_H
#define SCT_VECSLICE_H

#include "vector.h"

typedef struct {
    const sct_vector_t *src_vec;
    size_t        start_index;
    size_t        size;
    size_t        _item_size;
} sct_vecslice_t;

int sct_vecslice_init(sct_vecslice_t *slice, const sct_vector_t *src_vec, size_t index, size_t size);

void *sct_vecslice_get(const sct_vecslice_t *slice, size_t index);

int sct_vecslice_extend(sct_vecslice_t *slice, size_t size);

int sct_vecslice_advance(sct_vecslice_t *slice, size_t size);

size_t sct_vecslice_size(const sct_vecslice_t *slice);

#endif
