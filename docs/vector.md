# Vector
## Gen macro
```C
// Ty - type of vector
// ExSz - size by which vector will be extended on each overflow
genvector(Ty, ExSz);
```

## Functions
```C
typedef struct {
    // array of values
    Ty  *arr;
    // used size of array
    size_t size;
    // alocated size of array
    size_t aloc_size;
} vector_##Ty##_t;
```

```C
// create and return empty vector
vector_##Ty##_t *vector_##Ty##_create();
```
```C
// push back value
// vec - vector
// val - value
void vector_##Ty##_push_back(vector_##Ty##_t *vec, Ty val);
```
```C
// delete vector and free memory witch been alocated for it
void vector_##Ty##_free(vector_##Ty##_t *vec);
```