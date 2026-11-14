# SimpleCTypes (SCT)

A minimalistic C99 library of containers, allocators and zero-allocation views. The implementation uses byte-addressed buffers (`u8*`) internally, so pointer arithmetic is never performed on `void*`.

## Features

- O(1) amortized vector growth with a step of `4 * item_size` bytes.
- O(1) list append through a tail pointer and node reuse after removal.
- Lazy list sentinels, including arena-backed lists.
- Bucket-based hash maps with direct bucket iteration.
- Dynamically resized arena hash maps with a 75% load-factor threshold.
- Arena allocation with zero-initialization, arbitrary power-of-two alignment and realloc-style copying.
- Geometric string growth and formatted or byte-oriented string operations.
- Safe empty slices and bounds checks without index overflow.
- Consistent state reset after `deinit`.

## Containers

### Dynamic vector

```c
sct_vector_t vec;
sct_vector_init(&vec, sizeof(int));

int value = 42;
sct_vector_push(&vec, &value);
sct_vector_reserve(&vec, 128);

int *item = sct_vector_get(&vec, 0);
int *last = sct_vector_pop(&vec);
sct_vector_swap_remove(&vec, 0);
sct_vector_deinit(&vec);
```

`cap` is measured in bytes. Every automatic expansion adds `4 * _item_size` bytes. `sct_vector_capacity` returns the number of complete items that fit in the current buffer. `sct_vector_pop` returns `NULL` for an empty vector.

### Arena vector

```c
sct_arena_t arena;
sct_arena_init(&arena);

sct_arena_vector_t vec;
sct_arena_vector_init(&vec, &arena, sizeof(double));

double value = 1.5;
sct_arena_vector_push(&vec, &value);
sct_arena_vector_reserve(&vec, 256);
sct_arena_vector_deinit(&vec);
sct_arena_deinit(&arena);
```

Arena vector expansion uses the same `4 * item_size` step. Previously allocated buffers remain owned by the arena until the arena is reset or destroyed.

### Vector slices

```c
sct_vector_t source;
sct_vector_init(&source, sizeof(int));

int value = 42;
sct_vector_push(&source, &value);

sct_vecslice_t slice;
sct_vecslice_init(&slice, &source, 0, 1);
int *item = sct_vecslice_get(&slice, 0);
sct_vecslice_advance(&slice, 1);
```

A slice may be empty at `source.size`. Range checks use subtraction and do not evaluate overflowing `index + size` expressions. `extend` grows the end of the slice; `advance` moves the beginning forward and clips the remaining size.

### Memory arena

```c
sct_arena_t arena;
sct_arena_init(&arena);

void *zeroed = sct_arena_alloc_zero(&arena, 32);
void *aligned = sct_arena_alloc_aligned(&arena, 64, 32);
void *grown = sct_arena_realloc(arena, zeroed, 32, 128);

sct_arena_free(&arena);
sct_arena_deinit(&arena);
```

`alloc_aligned` requires a nonzero power-of-two alignment. The arena uses over-allocation to provide alignments larger than the native malloc alignment. `realloc` copies `min(old_size, new_size)` bytes and does not reclaim the old allocation. The arena has no checkpoint API; `sct_arena_free` resets all blocks when they are below the configured retention limit, otherwise it recreates the arena.

### List

```c
sct_list_t list;
sct_list_init(&list, sizeof(int));

int value = 10;
sct_list_push(&list, &value);

sct_list_iter_t iter;
sct_list_iter_init(&iter, &list);
while (sct_list_iter_next(&iter)) {
    int *item = sct_list_iter_value(&iter);
    if (*item == 10) {
        sct_list_iter_erase(&iter);
    }
}
sct_list_deinit(&list);
```

`push` is O(1) because the list stores its tail. Removed nodes are retained in a per-list reuse pool until `deinit`. The iterator erase operation is O(1) and supports erasing the current node during traversal. List sentinels are created lazily.

### Arena list

```c
sct_arena_list_t list;
sct_arena_list_init(&list, &arena, sizeof(int));

int value = 10;
sct_arena_list_push(&list, &value);
sct_arena_list_clear(&list);
sct_arena_list_deinit(&list);
```

Arena lists use lazy sentinels and O(1) append. `clear` removes the list logically; the memory remains owned by the arena. The arena-list iterator supports O(1) current-node removal without freeing arena memory.

### Hash map

```c
sct_hashmap_t map;
sct_hashmap_init(&map, sizeof(int));

int value = 999;
sct_hashmap_add(&map, "username", &value);
int *item = sct_hashmap_get(&map, "username");

sct_hashmap_iter_t iter;
sct_hashmap_iter_init(&iter, &map);
while (sct_hashmap_iter_next(&iter)) {
    const char *key = sct_hashmap_iter_key(&iter);
    void *data = sct_hashmap_iter_value(&iter);
}
```

The iterator scans each bucket and its collision chain once. It does not hash keys or use the key-tracking vector. Structural changes during an active traversal are not supported.

### Arena hash map

```c
sct_arena_hashmap_t map;
sct_arena_hashmap_init(&map, &arena, sizeof(int));

int value = 10;
sct_arena_hashmap_add(&map, "key", &value);
void *item = sct_arena_hashmap_get(&map, "key");
sct_arena_hashmap_remove(&map, "key");
```

The arena hash map has no duplicate key vector. It starts with 128 buckets, grows by doubling at a 75% load factor, and rehashes existing arena-owned nodes without copying entry values. `deinit` resets the map state but does not release the external arena.

### Formatted string

```c
sct_string_t str;
sct_string_init(&str);

sct_string_append(&str, "User: ", 6);
sct_string_cat(&str, "%s", "ID_TEST");
sct_string_insert(&str, 6, "[%s] ", "ADMIN");
sct_string_erase(&str, 0, 3);
sct_string_truncate(&str, 10);
sct_string_reserve(&str, 256);
sct_string_deinit(&str);
```

String storage grows geometrically. `sct_string_erase` and `sct_string_replace` use a half-open range `[start, end)`. `replace` swaps reversed bounds before applying the replacement.

### Formatting and alignment

```c
char buffer[64];
int result = sct_format(buffer, sizeof(buffer), "%s %d", "value", 10);

size_t aligned;
int checked = sct_align_up_checked(17, 8, &aligned);
```

`amalloc` aborts on allocation failure and prints the requested size. `sct_align_up_checked` reports invalid alignment and overflow without wrapping.

## Project structure

```text
include/SCT/
├── common.h
├── vector.h
├── arena_vector.h
├── vecslice.h
├── arena.h
├── list.h
├── arena_list.h
├── hashmap.h
├── arena_hashmap.h
└── string.h
src/
├── common.c
├── vector.c
├── arena_vector.c
├── vecslice.c
├── arena.c
├── list.c
├── arena_list.c
├── hashmap.c
├── arena_hashmap.c
└── string.c
tests/
└── test_all.c
```

## Build

```text
make
make MODE=debug
make MODE=release-size
```

The library targets C99 and has no external dependencies.
