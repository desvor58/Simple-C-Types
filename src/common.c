#include <SCT/common.h>

void *amalloc(size_t size)
{
    void *ret = malloc(size);
    if (!ret) abort();
    return ret;
}

int sct_format(char *buf, size_t buf_size, const char *fmt, ...)
{
    va_list args1, args2;
    
    va_start(args1, fmt);
    va_copy(args2, args1);

    int size = vsnprintf(NULL, 0, fmt, args1);
    va_end(args1);

    if (size < 0) {
        va_end(args2);
        return 1;
    }

    if (buf_size < (size_t)size + 1) {
        va_end(args2);
        return 1;
    }

    vsnprintf(buf, buf_size, fmt, args2);
    va_end(args2);

    return 0;
}
