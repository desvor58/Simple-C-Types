# String
## Gen macro
Does not require generate

## Functions
```C
typedef struct
{
    // C-style string
    char  *str;
    // used size of alocated string
    size_t size;
    // real alocated string size
    size_t aloc_size;
} string_t;
```

```C
// create and return empty string
string_t *string_create();
```
```C
// push back of string char
// str - string object
// c - char to push
void string_push_back(string_t *str, char c);
```
```C
// push back of string C-style formated string
// str - string object
// fmt - format string
// ... - objects for foramt
void string_cat(string_t *str, char *fmt, ...);
```
```C
// instert to string C-style formated string
// str - string object
// index - index after witch will be insert C-style string
// fmt - format string
// ... - objects for format
void string_insert(string_t *str, size_t index, char *fmt, ...);
```
```C
// replace part of string another C-style formated string
// str - string object
// start - index from which replacement will begin
// end - index at which replacement will end
// fmt - format string
// ... - objects fo format
void string_replace(string_t *str, size_t start, size_t end, char *fmt, ...);
```
```C
// delete string and free memory witch been alocated for it
void string_free(string_t *str)
```