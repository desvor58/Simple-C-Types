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

## Types
- [list](docs/list.md)
- [vector](docs/vector.md)
- [string](docs/string.md)

## Examples
```C
genlist(int);
genvector(float, 10);

int foo()
{
    // creating list
    list_int_pair_t *my_list = list_int_create();

    // append to list 100 integers from 0 to 99
    for (int i = 0; i < 100; i++) {
        int *bucket = malloc(sizeof(int));
        bucket = i;
        list_int_add(my_list, buckit);
    }

    // print list values
    foreach (list_int_pair_t, my_list) {
        printf_s("list: %d\n", *cur->val);
    }

    // delete list with values
    list_int_full_free(my_list);


    // creating vector
    vector_float_t *my_vec = vector_float_create();

    // append to vector 100 integers floar 0 to 99
    for (int i = 0; i < 100; i++) {
        vector_float_push_back(my_vec, (float)(i / 2));
    }

    // print vectors values
    for (size_t i = 0; i < my_vec->size; i++) {
        printf_s("vec: %d\n", my_vec->arr[i]);
    }

    // delete vector
    vector_float_free(my_vec);
}
```