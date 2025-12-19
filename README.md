# Simple-C-Types (SCT)
Simple implementation of data structure on C

## generation macros
SCT types can work with all types.

For declaration SCT-type for you type you need call the generation macro.

## Naming
All SCT types methods named like:
```
<SCT-type-name>_<type>_<mathod-name>
```

For Example:
```
list_int_create  // for creating list with type int
list_int_get     // for getting int value from list

vector_float_create     // creating vector with type float
vector_MyStruct_delete  // delete int value from vector of type MyStruct
```

## List
### Gen macro
```C
genlist(<type>);
```

### Functions
List pair structure

Note: *Ty* in declarations - type for SCT type

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