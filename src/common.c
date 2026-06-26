#include <common.h>

void *amalloc(size_t size)
{
    void *ret = malloc(size);
    if (!ret) abort();
    return ret;
}

