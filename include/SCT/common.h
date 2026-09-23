#ifndef SCT_COMMON_H
#define SCT_COMMON_H

#define _CRT_SECURE_NO_WARNINGS

#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef int8_t   i8;
typedef uint8_t  u8;
typedef int16_t  i16;
typedef uint16_t u16;
typedef int32_t  i32;
typedef uint32_t u32;
typedef int64_t  i64;
typedef uint64_t u64;
typedef float    f32;
typedef double   f64;

void *amalloc(size_t size);

int sct_format(char *buf, size_t buf_size, const char *fmt, ...);

int sct_align_up_checked(size_t size, size_t alignment, size_t *result);

static inline size_t sct_align_up(size_t size)
{
    size_t result;
    if (sct_align_up_checked(size, 8, &result)) {
        abort();
    }
    return result;
}

#endif