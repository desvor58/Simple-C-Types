# List
## Gen macro
```C
// Ty - type of list
genlist(Ty);
```

## Functions
```C
struct list_##Ty##_pair_t {
    // ptr to value
    Ty                        *val;
    // ptr to next list pair
    struct list_##Ty##_pair_t *next;
};
```

```C
// create and return start list pair
// to end of work with list call list_##Ty##_free or list_##Ty##_full_free
list_##Ty##_pair_t *list_##Ty##_create();
```
```C
// append to end of list ptr to value
// start - start pair of list, created with list_##Ty##_create
// val - ptr to value
// return 0 if all good, else return not 0
int list_##Ty##_add(list_##Ty##_pair_t *start, Ty *val);
```
```C
// calculate list size
// start - start pair of list, created with list_##Ty##_create
// return size of list or -1 if smthng will go wrong
long long list_##Ty##_size(list_##Ty##_pair_t *start);
```
```C
// set value by index
// start - start pair of list, created with list_##Ty##_create
// index - index of element to set
// val - value for set
// return 0 if all good, else return not 0
int list_##Ty##_set(list_##Ty##_pair_t *start, size_t index, Ty *val);
```
```C
// get value by index
// start - start pair of list, created with list_##Ty##_create
// index - index of element to get
// return ptr to value if all good, else return 0
Ty *list_##Ty##_get(list_##Ty##_pair_t *start, size_t index);
```
```C
// delete element by index
// start - start pair of list, created with list_##Ty##_create
// index - index of element to delete
// return ptr to new start of list or 0 if detected an error
list_##Ty##_pair_t *list_##Ty##_delete(list_##Ty##_pair_t *start, size_t index);
```
```C
// delete list and free memory witch been alocated for it
// start - start pair of list, created with list_##Ty##_create
// return 0 if all good, else return not 0
int list_##Ty##_free(list_##Ty##_pair_t *start);
```
```C
// delete list and free memory witch been alocated for it and free all values
// start - start pair of list, created with list_##Ty##_create
// return 0 if all good, else return not 0
int list_##Ty##_full_free(list_##Ty##_pair_t *start);
```

## Foreach
SCT have special macro for working with list - foreach

```C
// LtTy - type of list pair (for example: list_int_pair_t / list_MyStruct_pair_t)
// list - list object of LtTy type
foreach (LtTy, list) {<body>}
```

In foreach body you can use variable *cur*. It point to current listpair.


```C
LtTy *cur;
```