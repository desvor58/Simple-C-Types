#include <SCT/string.h>

void sct_string_init(sct_string_t *str)
{
    str->cstr = amalloc(SCT_STRING_ALLOC_SIZE);
    str->cstr[0] = '\0';
    str->size = 0;
    str->cap = SCT_STRING_ALLOC_SIZE;
}

void sct_string_deinit(sct_string_t *str)
{
    free(str->cstr);
}

static void sct_string_realloc(sct_string_t *str, size_t size)
{
    str->cap += (size * sizeof(char) / SCT_STRING_ALLOC_SIZE + 1) * SCT_STRING_ALLOC_SIZE;
    char *new_cstr = amalloc(str->cap);
    if (str->size > 0) {
        memcpy(new_cstr, str->cstr, str->size);
    }
    free(str->cstr);
    str->cstr = new_cstr;
}

void sct_string_push(sct_string_t *str, char c)
{
    if (str->size + 2 > str->cap) {
        sct_string_realloc(str, 1);
    }
    str->cstr[str->size++] = c;
    str->cstr[str->size] = '\0';
}

void sct_string_cat(sct_string_t *str, char *fmt, ...)
{
    va_list args;
    
    va_start(args, fmt);
    int formatted_len = vsnprintf(0, 0, fmt, args);
    va_end(args);
    
    if (formatted_len <= 0) return;
    
    if (str->size + formatted_len + 1 > str->cap) {
        sct_string_realloc(str, formatted_len);
    }
    
    va_start(args, fmt);
    vsnprintf(str->cstr + str->size, str->cap - str->size, fmt, args);
    va_end(args);

    str->size += formatted_len;
}

void sct_string_insert(sct_string_t *str, size_t index, char *fmt, ...)
{
    if (index > str->size) return;

    va_list args;
    
    va_start(args, fmt);
    int formatted_len = vsnprintf(NULL, 0, fmt, args);
    va_end(args);
    
    if (formatted_len <= 0) return;
    
    if (str->size + formatted_len + 1 > str->cap) {
        sct_string_realloc(str, formatted_len);
    }
    
    size_t bytes_to_move = str->size - index + 1;
    memmove(str->cstr + index + formatted_len, str->cstr + index, bytes_to_move);
    
    va_start(args, fmt);
    vsnprintf(str->cstr + index, formatted_len + 1, fmt, args);
    va_end(args);
    
    str->size += formatted_len;
}

void sct_string_replace(sct_string_t *str, size_t start, size_t end, char *fmt, ...)
{
    if (start > str->size) start = str->size;
    if (end > str->size) end = str->size;
    if (start > end) {
        size_t tmp = start;
        start = end;
        end = tmp;
    }

    size_t replaced_len = end - start;

    va_list args;
    
    va_start(args, fmt);
    int formatted_len = vsnprintf(NULL, 0, fmt, args);
    va_end(args);
    
    if (formatted_len < 0) return;

    i64 delta = (i64)formatted_len - (i64)replaced_len;
    if (delta > 0 && str->size + delta + 1 > str->cap) {
        sct_string_realloc(str, (size_t)delta);
    }

    memmove(str->cstr + end + delta, str->cstr + end, str->size - end + 1);

    if (formatted_len > 0) {
        va_start(args, fmt);
        vsnprintf(str->cstr + start, (size_t)formatted_len + 1, fmt, args);
        va_end(args);
    }

    str->size = str->size + delta;
    str->cstr[str->size] = '\0';
}