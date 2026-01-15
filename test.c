#include <stdio.h>
#include "include/SCT/hashmap.h"

int main()
{
    int *val;
    sct_hashmap_t *map = sct_hashmap_create();
    int *i = malloc(sizeof(int));
    sct_hashmap_set(map, "key", i);
    val = sct_hashmap_get(map, "key");
    printf("key:%u\n", val);

    int *j = malloc(sizeof(int));
    sct_hashmap_set(map, "key2", j);
    val = sct_hashmap_get(map, "key");
    printf("key2:%u\n", val);

    sct_hashmap_delete(map, "key");
    sct_hashmap_delete(map, "key2");
    
    val = sct_hashmap_get(map, "key");
    printf("k:%u\n", val);
    val = sct_hashmap_get(map, "key");
    printf("k2:%u\n", val);
}