# SimpleCTypes (SCT)

A minimalistic, fast, and secure library providing core data structures and memory allocators in pure C99. It is engineered to minimize memory fragmentation, maximize performance, and provide total control over memory allocations.

## 🚀 Features
* **Abort-on-OOM**: All containers are built on top of `amalloc`—a controlled `abort()` with error logging that triggers if the system runs out of memory.
* **Cache-Locality & ARM Protection**: The built-in memory arena prevents heap fragmentation and eliminates memory alignment faults (*Bus errors*) on ARM architectures (Apple M-series, smartphones, Raspberry Pi).
* **O(1) Operations**: Singly-linked list features instant `push` via a tracking tail pointer, while the hash map ensures `O(1)` lookups using the FNV-1a hashing algorithm.
* **Strict Portability**: Designed around strict pointer arithmetic compliant with the ISO C standard by managing internal buffers via `u8*` (byte-level offsets).

---

## 🛠️ Usage Examples (Quick Start)

### 1. Dynamic Vector (`sct_vector_t`)
```c
sct_vector_t vec;
sct_vector_init(&vec, sizeof(int));

int value = 42;
sct_vector_push(&vec, &value);

int *res = (int*)sct_vector_get(&vec, 0);

sct_vector_erase(&vec, 0);

sct_vector_deinit(&vec);
```

### 2. Memory Arena (`sct_arena_t`)
```c
sct_arena_t arena;
sct_arena_init(&arena);

// Bump allocation with built-in 8-byte boundary alignment
char *buffer = sct_arena_alloc(&arena, 100);

// Instant reset of all allocated memory (shrinks to the base chunk size if limits are exceeded)
sct_arena_free(&arena); 

sct_arena_deinit(&arena);
```

### 3. Singly-Linked List (`sct_list_t`)
```c
sct_list_t list;
sct_list_init(&list, sizeof(double));

double pi = 3.1415;
sct_list_push(&list, &pi);

// Iterate through elements using the built-in macro loop
foreach(&list) {
    double *val = sct_get_val(cur_pair, double);
}

sct_list_erase(&list, 0);
sct_list_deinit(&list);
```

### 4. Hash Map (`sct_hashmap_t`)
```c
sct_hashmap_t map;
sct_hashmap_init(&map, sizeof(int));

int user_id = 999;
sct_hashmap_add(&map, "username", &user_id);

if (sct_hashmap_contains(&map, "username")) {
    int *id = sct_hashmap_get(&map, "username");
}

sct_hashmap_remove(&map, "username");
sct_hashmap_deinit(&map);
```

---

---

## 📜 License
This project is licensed under the MIT License. For granular implementation specifics, internal memory layout mechanics, and full function documentations, check out our **GitHub Wiki**.
