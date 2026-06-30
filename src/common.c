#include <SCT/common.h>

void *amalloc(size_t size)
{
    void *ret = malloc(size);
    if (!ret) abort();
    return ret;
}

char *sct_format(const char *fmt, ...)
{
    va_list args1, args2;
    
    va_start(args1, fmt);
    va_copy(args2, args1);

    int size = vsnprintf(NULL, 0, fmt, args1);
    va_end(args1);

    if (size < 0) {
        va_end(args2);
        return NULL;
    }

    char *str = amalloc(size + 1);
    if (!str) {
        va_end(args2);
        return NULL;
    }

    vsnprintf(str, size + 1, fmt, args2);
    va_end(args2);

    return str;
}
