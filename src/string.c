#include <SCT/string.h>

static int string_required_capacity(size_t size, size_t extra, size_t *result)
{
    if (size > SIZE_MAX - 1 || extra > SIZE_MAX - size - 1) {
        return 1;
    }
    *result = size + extra + 1;
    return 0;
}

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
    str->cstr = NULL;
    str->size = 0;
    str->cap = 0;
}

int sct_string_reserve(sct_string_t *str, size_t capacity)
{
    size_t new_cap;
    char *new_cstr;
    if (!str) {
        return 1;
    }
    if (!capacity || capacity <= str->cap) {
        return 0;
    }
    new_cap = str->cap ? str->cap : SCT_STRING_ALLOC_SIZE;
    while (new_cap < capacity) {
        if (new_cap > SIZE_MAX / 2) {
            new_cap = capacity;
            break;
        }
        new_cap *= 2;
    }
    new_cstr = realloc(str->cstr, new_cap);
    if (!new_cstr) {
        fprintf(stderr, "string reserve to %zu bytes failed\n", new_cap);
        abort();
    }
    str->cstr = new_cstr;
    str->cap = new_cap;
    return 0;
}

void sct_string_push(sct_string_t *str, char c)
{
    if (str->size > SIZE_MAX - 2 || sct_string_reserve(str, str->size + 2)) {
        abort();
    }
    str->cstr[str->size++] = c;
    str->cstr[str->size] = '\0';
}

int sct_string_append(sct_string_t *str, const char *data, size_t size)
{
    size_t required;
    if (!str || !str->cstr || (!data && size) ||
        string_required_capacity(str->size, size, &required)) {
        return 1;
    }
    if (size && sct_string_reserve(str, required)) {
        return 1;
    }
    if (size) {
        memmove(str->cstr + str->size, data, size);
        str->size += size;
    }
    str->cstr[str->size] = '\0';
    return 0;
}

void sct_string_cat(sct_string_t *str, const char *fmt, ...)
{
    va_list args;
    size_t required;
    int formatted_len;
    va_start(args, fmt);
    formatted_len = vsnprintf(NULL, 0, fmt, args);
    va_end(args);
    if (formatted_len <= 0) {
        return;
    }
    if (string_required_capacity(str->size, (size_t)formatted_len, &required) ||
        sct_string_reserve(str, required)) {
        abort();
    }
    va_start(args, fmt);
    vsnprintf(str->cstr + str->size, str->cap - str->size, fmt, args);
    va_end(args);
    str->size += (size_t)formatted_len;
}

void sct_string_insert(sct_string_t *str, size_t index, const char *fmt, ...)
{
    va_list args;
    size_t required;
    char boundary;
    int formatted_len;
    if (index > str->size) {
        return;
    }
    va_start(args, fmt);
    formatted_len = vsnprintf(NULL, 0, fmt, args);
    va_end(args);
    if (formatted_len <= 0) {
        return;
    }
    if (string_required_capacity(str->size, (size_t)formatted_len, &required) ||
        sct_string_reserve(str, required)) {
        abort();
    }
    memmove(str->cstr + index + (size_t)formatted_len,
            str->cstr + index,
            str->size - index + 1);
    boundary = str->cstr[index + (size_t)formatted_len];
    va_start(args, fmt);
    vsnprintf(str->cstr + index, (size_t)formatted_len + 1, fmt, args);
    va_end(args);
    str->cstr[index + (size_t)formatted_len] = boundary;
    str->size += (size_t)formatted_len;
    str->cstr[str->size] = '\0';
}

void sct_string_replace(sct_string_t *str, size_t start, size_t end, const char *fmt, ...)
{
    va_list args;
    size_t replaced_len;
    size_t formatted_size;
    size_t growth;
    size_t required;
    char boundary;
    int formatted_len;
    if (start > str->size) {
        start = str->size;
    }
    if (end > str->size) {
        end = str->size;
    }
    if (start > end) {
        size_t tmp = start;
        start = end;
        end = tmp;
    }
    va_start(args, fmt);
    formatted_len = vsnprintf(NULL, 0, fmt, args);
    va_end(args);
    if (formatted_len < 0) {
        return;
    }
    formatted_size = (size_t)formatted_len;
    replaced_len = end - start;
    if (formatted_size > replaced_len) {
        growth = formatted_size - replaced_len;
        if (string_required_capacity(str->size, growth, &required) ||
            sct_string_reserve(str, required)) {
            abort();
        }
        memmove(str->cstr + end + growth,
                str->cstr + end,
                str->size - end + 1);
    } else {
        memmove(str->cstr + start + formatted_size,
                str->cstr + end,
                str->size - end + 1);
    }
    boundary = str->cstr[start + formatted_size];
    if (formatted_size) {
        va_start(args, fmt);
        vsnprintf(str->cstr + start, formatted_size + 1, fmt, args);
        va_end(args);
        str->cstr[start + formatted_size] = boundary;
    } else {
        str->cstr[start] = boundary;
    }
    str->size = str->size - replaced_len + formatted_size;
    str->cstr[str->size] = '\0';
}

int sct_string_erase(sct_string_t *str, size_t start, size_t end)
{
    size_t erased_len;
    if (!str || !str->cstr) {
        return 1;
    }
    if (start > str->size) {
        start = str->size;
    }
    if (end > str->size) {
        end = str->size;
    }
    if (start > end) {
        size_t tmp = start;
        start = end;
        end = tmp;
    }
    erased_len = end - start;
    memmove(str->cstr + start, str->cstr + end, str->size - end + 1);
    str->size -= erased_len;
    str->cstr[str->size] = '\0';
    return 0;
}

int sct_string_truncate(sct_string_t *str, size_t size)
{
    if (!str || !str->cstr || size > str->size) {
        return 1;
    }
    str->size = size;
    str->cstr[str->size] = '\0';
    return 0;
}

size_t sct_string_size(const sct_string_t *str)
{
    return str ? str->size : 0;
}

size_t sct_string_capacity(const sct_string_t *str)
{
    return str ? str->cap : 0;
}
