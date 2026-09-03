#include <SCT/arena.h>
#include <SCT/arena_vector.h>
#include <SCT/arena_list.h>
#include <SCT/arena_hashmap.h>
#include <SCT/vector.h>
#include <SCT/list.h>
#include <SCT/hashmap.h>
#include <SCT/string.h>
#include <SCT/vecslice.h>
#include <stdio.h>
#include <string.h>

static int tests_passed = 0;
static int tests_failed = 0;

#define ASSERT(cond, msg) do { \
    if (!(cond)) { \
        printf("  FAIL: %s (%s:%d)\n", msg, __FILE__, __LINE__); \
        tests_failed++; \
    } else { \
        printf("  PASS: %s\n", msg); \
        tests_passed++; \
    } \
} while(0)

/* ==================== ARENA TESTS ==================== */
static void test_arena(void)
{
    printf("\n--- Arena Tests ---\n");

    sct_arena_t arena;
    sct_arena_init(&arena);
    ASSERT(arena.arena != NULL, "arena init - arena not null");
    ASSERT(arena.size == 0, "arena init - size == 0");
    ASSERT(arena.cap > 0, "arena init - cap > 0");

    void *p1 = sct_arena_alloc(&arena, 8);
    ASSERT(p1 != NULL, "arena alloc 8 bytes - not null");
    ASSERT(arena.size >= 8, "arena alloc 8 bytes - size increased");

    void *p2 = sct_arena_alloc(&arena, 100);
    ASSERT(p2 != NULL, "arena alloc 100 bytes - not null");
    ASSERT(p2 != p1, "arena alloc - different pointers");

    void *p3 = sct_arena_alloc(&arena, 3000);
    ASSERT(p3 != NULL, "arena alloc 3000 bytes (triggers realloc) - not null");

    memset(p3, 0xAB, 3000);
    ASSERT(((u8*)p3)[0] == 0xAB, "arena alloc - p3 writeable after realloc");
    ASSERT(((u8*)p3)[2999] == 0xAB, "arena alloc - p3 end writeable after realloc");

    sct_arena_free(&arena);
    ASSERT(arena.size == 0, "arena free - size reset to 0");

    void *p4 = sct_arena_alloc(&arena, 16);
    ASSERT(p4 != NULL, "arena alloc after free - not null");
    ASSERT(arena.size > 0, "arena alloc after free - size > 0");

    sct_arena_deinit(&arena);
}

/* ==================== VECTOR TESTS ==================== */
static void test_vector(void)
{
    printf("\n--- Vector Tests ---\n");

    sct_vector_t vec;
    sct_vector_init(&vec, sizeof(int));
    ASSERT(vec.data != NULL, "vector init - data not null");
    ASSERT(vec.size == 0, "vector init - size == 0");

    int a = 42;
    sct_vector_push(&vec, &a);
    ASSERT(vec.size == 1, "vector push 1 - size == 1");

    int b = 99;
    sct_vector_push(&vec, &b);
    ASSERT(vec.size == 2, "vector push 2 - size == 2");

    int *g = (int*)sct_vector_get(&vec, 0);
    ASSERT(g != NULL && *g == 42, "vector get(0) == 42");
    g = (int*)sct_vector_get(&vec, 1);
    ASSERT(g != NULL && *g == 99, "vector get(1) == 99");
    ASSERT(sct_vector_get(&vec, 5) == NULL, "vector get(5) == NULL (out of bounds)");

    g = (int*)sct_vector_pop(&vec);
    ASSERT(g != NULL && *g == 99, "vector pop == 99");
    ASSERT(vec.size == 1, "vector pop - size == 1");

    /* Vector insert */
    int c = 55;
    sct_vector_insert(&vec, 0, &c);
    ASSERT(vec.size == 2, "vector insert(0) - size == 2");
    g = (int*)sct_vector_get(&vec, 0);
    ASSERT(g != NULL && *g == 55, "vector insert(0) - get(0) == 55");
    g = (int*)sct_vector_get(&vec, 1);
    ASSERT(g != NULL && *g == 42, "vector insert(0) - get(1) == 42");

    /* Insert at end via push (index == size - 1) */
    sct_vector_insert(&vec, 1, &a);
    ASSERT(vec.size == 3, "vector insert(1) - size == 3");

    /* Vector erase */
    sct_vector_erase(&vec, 0);
    ASSERT(vec.size == 2, "vector erase(0) - size == 2");
    g = (int*)sct_vector_get(&vec, 0);
    ASSERT(g != NULL && *g == 42, "vector erase(0) - get(0) == 42");

    sct_vector_erase(&vec, 5); /* out of bounds, should not crash */
    ASSERT(vec.size == 2, "vector erase(5) - size unchanged");

    sct_vector_deinit(&vec);
}

/* ==================== LIST TESTS ==================== */
static void test_list(void)
{
    printf("\n--- List Tests ---\n");

    sct_list_t list;
    sct_list_init(&list, sizeof(int));
    ASSERT(list.first_pair != NULL, "list init - first_pair not null");
    ASSERT(list.size == 0, "list init - size == 0");

    int a = 10, b = 20, c = 30;
    sct_list_push(&list, &a);
    ASSERT(list.size == 1, "list push 1 - size == 1");

    sct_list_push(&list, &b);
    ASSERT(list.size == 2, "list push 2 - size == 2");

    sct_list_push(&list, &c);
    ASSERT(list.size == 3, "list push 3 - size == 3");

    int *v = (int*)sct_list_get(&list, 0);
    ASSERT(v != NULL && *v == 10, "list get(0) == 10");
    v = (int*)sct_list_get(&list, 1);
    ASSERT(v != NULL && *v == 20, "list get(1) == 20");
    v = (int*)sct_list_get(&list, 2);
    ASSERT(v != NULL && *v == 30, "list get(2) == 30");
    ASSERT(sct_list_get(&list, 5) == NULL, "list get(5) == NULL (out of bounds)");

    /* Test foreach now processes all elements */
    int count = 0, sum = 0;
    foreach(&list) {
        int val = *(int*)((u8*)cur_pair + sizeof(void*));
        sum += val;
        count++;
    }
    ASSERT(count == 3, "list foreach - visited all 3 elements");
    ASSERT(sum == 60, "list foreach - sum = 10+20+30 = 60");

    /* Test erase */
    sct_list_erase(&list, 0);
    ASSERT(list.size == 2, "list erase(0) - size == 2");
    v = (int*)sct_list_get(&list, 0);
    ASSERT(v != NULL && *v == 20, "list erase(0) - get(0) == 20");

    sct_list_erase(&list, 1);
    ASSERT(list.size == 1, "list erase(1) - size == 1");
    v = (int*)sct_list_get(&list, 0);
    ASSERT(v != NULL && *v == 20, "list erase(1) - get(0) == 20");

    sct_list_erase(&list, 0);
    ASSERT(list.size == 0, "list erase last - size == 0");

    /* Push after all erased */
    int d = 99;
    sct_list_push(&list, &d);
    ASSERT(list.size == 1, "list push after erase - size == 1");
    v = (int*)sct_list_get(&list, 0);
    ASSERT(v != NULL && *v == 99, "list push after erase - get(0) == 99");

    sct_list_deinit(&list);
}

/* ==================== HASHMAP TESTS ==================== */
static void test_hashmap(void)
{
    printf("\n--- Hashmap Tests ---\n");

    sct_hashmap_t map;
    sct_hashmap_init(&map, sizeof(int));

    int v1 = 100, v2 = 200, v3 = 300;

    ASSERT(sct_hashmap_contains(&map, "key1") == 0, "hashmap contains 'key1' initially false");

    sct_hashmap_add(&map, "key1", &v1);
    ASSERT(sct_hashmap_contains(&map, "key1") == 1, "hashmap contains 'key1' after add");

    sct_hashmap_add(&map, "key2", &v2);
    sct_hashmap_add(&map, "key3", &v3);

    int *g = (int*)sct_hashmap_get(&map, "key1");
    ASSERT(g != NULL && *g == 100, "hashmap get key1 == 100");
    g = (int*)sct_hashmap_get(&map, "key2");
    ASSERT(g != NULL && *g == 200, "hashmap get key2 == 200");
    g = (int*)sct_hashmap_get(&map, "key3");
    ASSERT(g != NULL && *g == 300, "hashmap get key3 == 300");
    ASSERT(sct_hashmap_get(&map, "nonexistent") == NULL, "hashmap get nonexistent == NULL");

    /* Update existing key */
    int v1_new = 999;
    sct_hashmap_add(&map, "key1", &v1_new);
    g = (int*)sct_hashmap_get(&map, "key1");
    ASSERT(g != NULL && *g == 999, "hashmap update key1 == 999");

    /* Remove */
    sct_hashmap_remove(&map, "key2");
    ASSERT(sct_hashmap_contains(&map, "key2") == 0, "hashmap contains 'key2' after remove");
    ASSERT(sct_hashmap_contains(&map, "key1") == 1, "hashmap still contains 'key1' after remove");
    ASSERT(sct_hashmap_contains(&map, "key3") == 1, "hashmap still contains 'key3' after remove");

    sct_hashmap_deinit(&map);
}

/* ==================== STRING TESTS ==================== */
static void test_string(void)
{
    printf("\n--- String Tests ---\n");

    sct_string_t str;
    sct_string_init(&str);
    ASSERT(str.cstr != NULL, "string init - cstr not null");
    ASSERT(str.size == 0, "string init - size == 0");
    ASSERT(strcmp(str.cstr, "") == 0, "string init - empty string");

    sct_string_push(&str, 'H');
    ASSERT(str.size == 1, "string push - size == 1");
    ASSERT(str.cstr[0] == 'H', "string push - content 'H'");

    sct_string_push(&str, 'i');
    ASSERT(strcmp(str.cstr, "Hi") == 0, "string push - 'Hi'");

    sct_string_cat(&str, " %s %d", "world", 42);
    ASSERT(strcmp(str.cstr, "Hi world 42") == 0, "string cat - 'Hi world 42'");

    sct_string_insert(&str, 3, "BEUTIFUL ");
    ASSERT(strcmp(str.cstr, "Hi BEUTIFUL world 42") == 0, "string insert - 'Hi BEUTIFUL world 42'");

    /* replace */
    sct_string_replace(&str, 3, 12, "beautiful");
    ASSERT(strcmp(str.cstr, "Hi beautiful world 42") == 0, "string replace - 'Hi beautiful world 42'");

    /* replace start > end */
    sct_string_replace(&str, 15, 3, "BIG ");
    ASSERT(strcmp(str.cstr, "Hi BIG beautiful world 42") == 0, "string replace swapped - 'Hi BIG beautiful world 42'");

    /* Insert at end */
    sct_string_insert(&str, str.size, "!");
    ASSERT(strcmp(str.cstr, "Hi BIG beautiful world 42!") == 0, "string insert at end");

    sct_string_deinit(&str);
}

/* ==================== VECSLICE TESTS ==================== */
static void test_vecslice(void)
{
    printf("\n--- Vecslice Tests ---\n");

    sct_vector_t vec;
    sct_vector_init(&vec, sizeof(int));

    int nums[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    for (int i = 0; i < 10; i++) {
        sct_vector_push(&vec, &nums[i]);
    }

    sct_vecslice_t slice;
    int ret = sct_vecslice_init(&slice, &vec, 3, 4);
    ASSERT(ret == 0, "vecslice init success");
    ASSERT(slice.size == 4, "vecslice size == 4");

    int *g = (int*)sct_vecslice_get(&slice, 0);
    ASSERT(g != NULL && *g == 3, "vecslice get(0) == 3");
    g = (int*)sct_vecslice_get(&slice, 3);
    ASSERT(g != NULL && *g == 6, "vecslice get(3) == 6");
    ASSERT(sct_vecslice_get(&slice, 10) == NULL, "vecslice get(10) == NULL (out of bounds)");

    ret = sct_vecslice_extend(&slice, 2);
    ASSERT(ret == 0, "vecslice extend 2 - success");
    ASSERT(slice.size == 6, "vecslice extend - size == 6");
    g = (int*)sct_vecslice_get(&slice, 5);
    ASSERT(g != NULL && *g == 8, "vecslice after extend get(5) == 8");

    /* Test invalid init */
    ret = sct_vecslice_init(&slice, &vec, 8, 10);
    ASSERT(ret != 0, "vecslice init out of bounds returns error");

    sct_vector_deinit(&vec);
}

/* ==================== COMMON TESTS ==================== */
static void test_common(void)
{
    printf("\n--- Common Tests ---\n");

    char buf[64];
    int ret = sct_format(buf, sizeof(buf), "Hello %s! Score: %d", "Test", 100);
    ASSERT(ret == 0, "sct_format success");
    ASSERT(strcmp(buf, "Hello Test! Score: 100") == 0, "sct_format correct output");

    ret = sct_format(buf, 5, "Too long string here");
    ASSERT(ret != 0, "sct_format small buffer returns error");

    ASSERT(sct_align_up(1) == 8, "align_up(1) == 8");
    ASSERT(sct_align_up(8) == 8, "align_up(8) == 8");
    ASSERT(sct_align_up(9) == 16, "align_up(9) == 16");
    ASSERT(sct_align_up(0) == 0, "align_up(0) == 0");
}

/* ==================== ARENA VECTOR TESTS ==================== */
static void test_arena_vector(void)
{
    printf("\n--- Arena Vector Tests ---\n");

    sct_arena_t arena;
    sct_arena_init(&arena);

    sct_arena_vector_t vec;
    sct_arena_vector_init(&vec, &arena, sizeof(int));
    ASSERT(vec.arena == &arena, "arena vector init - arena saved");
    ASSERT(vec.data != NULL, "arena vector init - data not null");
    ASSERT(vec.size == 0, "arena vector init - size == 0");

    int a = 42, b = 99;
    sct_arena_vector_push(&vec, &a);
    sct_arena_vector_push(&vec, &b);
    ASSERT(vec.size == 2, "arena vector push - size == 2");

    int *g = (int*)sct_arena_vector_get(&vec, 0);
    ASSERT(g != NULL && *g == 42, "arena vector get(0) == 42");
    g = (int*)sct_arena_vector_get(&vec, 1);
    ASSERT(g != NULL && *g == 99, "arena vector get(1) == 99");
    ASSERT(sct_arena_vector_get(&vec, 5) == NULL, "arena vector get(5) == NULL (out of bounds)");

    /* Bulk push to trigger realloc */
    for (int i = 0; i < 200; i++) {
        sct_arena_vector_push(&vec, &i);
    }
    ASSERT(vec.size == 202, "arena vector bulk push - size == 202");
    g = (int*)sct_arena_vector_get(&vec, 201);
    ASSERT(g != NULL && *g == 199, "arena vector bulk push - last element correct");

    /* deinit must NOT release the arena */
    size_t arena_size_before = arena.size;
    sct_arena_vector_deinit(&vec);
    ASSERT(arena.size == arena_size_before, "arena vector deinit - arena not released");

    sct_arena_deinit(&arena);
}

/* ==================== ARENA LIST TESTS ==================== */
static void test_arena_list(void)
{
    printf("\n--- Arena List Tests ---\n");

    sct_arena_t arena;
    sct_arena_init(&arena);

    sct_arena_list_t list;
    sct_arena_list_init(&list, &arena, sizeof(int));
    ASSERT(list.arena == &arena, "arena list init - arena saved");
    ASSERT(list.first_pair != NULL, "arena list init - first_pair not null");
    ASSERT(list.size == 0, "arena list init - size == 0");

    int a = 10, b = 20, c = 30;
    sct_arena_list_push(&list, &a);
    sct_arena_list_push(&list, &b);
    sct_arena_list_push(&list, &c);
    ASSERT(list.size == 3, "arena list push - size == 3");

    int *v = (int*)sct_arena_list_get(&list, 0);
    ASSERT(v != NULL && *v == 10, "arena list get(0) == 10");
    v = (int*)sct_arena_list_get(&list, 2);
    ASSERT(v != NULL && *v == 30, "arena list get(2) == 30");
    ASSERT(sct_arena_list_get(&list, 5) == NULL, "arena list get(5) == NULL (out of bounds)");

    int count = 0, sum = 0;
    sct_arena_foreach(&list) {
        int val = *(int*)((u8*)cur_pair + sizeof(void*));
        sum += val;
        count++;
    }
    ASSERT(count == 3 && sum == 60, "arena list foreach - visited all elements");

    sct_arena_list_erase(&list, 0);
    ASSERT(list.size == 2, "arena list erase - size == 2");
    v = (int*)sct_arena_list_get(&list, 0);
    ASSERT(v != NULL && *v == 20, "arena list erase - elements shifted");

    /* deinit must NOT release the arena */
    size_t arena_size_before = arena.size;
    sct_arena_list_deinit(&list);
    ASSERT(arena.size == arena_size_before, "arena list deinit - arena not released");

    sct_arena_deinit(&arena);
}

/* ==================== ARENA HASHMAP TESTS ==================== */
static void test_arena_hashmap(void)
{
    printf("\n--- Arena Hashmap Tests ---\n");

    sct_arena_t arena;
    sct_arena_init(&arena);

    sct_arena_hashmap_t map;
    sct_arena_hashmap_init(&map, &arena, sizeof(int));
    ASSERT(map.arena == &arena, "arena hashmap init - arena saved");

    int v1 = 100, v2 = 200;
    sct_arena_hashmap_add(&map, "key1", &v1);
    sct_arena_hashmap_add(&map, "key2", &v2);
    ASSERT(sct_arena_hashmap_contains(&map, "key1") == 1, "arena hashmap contains key1");
    ASSERT(sct_arena_hashmap_contains(&map, "nope") == 0, "arena hashmap contains missing");

    int *g = (int*)sct_arena_hashmap_get(&map, "key1");
    ASSERT(g != NULL && *g == 100, "arena hashmap get key1 == 100");
    g = (int*)sct_arena_hashmap_get(&map, "key2");
    ASSERT(g != NULL && *g == 200, "arena hashmap get key2 == 200");

    sct_arena_hashmap_remove(&map, "key2");
    ASSERT(sct_arena_hashmap_contains(&map, "key2") == 0, "arena hashmap remove key2");
    ASSERT(sct_arena_hashmap_contains(&map, "key1") == 1, "arena hashmap still has key1");

    /* deinit must NOT release the arena */
    size_t arena_size_before = arena.size;
    sct_arena_hashmap_deinit(&map);
    ASSERT(arena.size == arena_size_before, "arena hashmap deinit - arena not released");

    sct_arena_deinit(&arena);
}

/* ==================== EDGE CASE TESTS ==================== */
static void test_edge_cases(void)
{
    printf("\n--- Edge Cases ---\n");

    /* Vector: empty operations */
    sct_vector_t vec;
    sct_vector_init(&vec, sizeof(int));
    ASSERT(sct_vector_get(&vec, 0) == NULL, "vector get from empty == NULL");
    sct_vector_erase(&vec, 0); /* should not crash */
    ASSERT(vec.size == 0, "vector erase from empty - size unchanged");
    sct_vector_deinit(&vec);

    /* List: empty operations */
    sct_list_t list;
    sct_list_init(&list, sizeof(int));
    ASSERT(sct_list_get(&list, 0) == NULL, "list get from empty == NULL");
    sct_list_erase(&list, 0); /* should not crash */
    ASSERT(list.size == 0, "list erase from empty - size unchanged");

    /* foreach on empty list should not crash and do nothing */
    int count = 0;
    foreach(&list) { count++; }
    ASSERT(count == 0, "foreach on empty list - count == 0");

    sct_list_deinit(&list);

    /* String: empty operations */
    sct_string_t str;
    sct_string_init(&str);
    sct_string_insert(&str, 5, "should not insert"); /* index > size */
    ASSERT(str.size == 0, "string insert beyond size - size unchanged");
    sct_string_replace(&str, 3, 1, "hello"); /* start > end swapped */
    sct_string_deinit(&str);

    /* Vector insert at end through push path */
    sct_vector_init(&vec, sizeof(double));
    double da = 1.0, db = 2.0;
    sct_vector_push(&vec, &da);
    sct_vector_push(&vec, &db);
    double dval = 1.5;
    sct_vector_insert(&vec, 1, &dval);
    ASSERT(vec.size == 3, "vector insert middle - size == 3");
    double *dg = (double*)sct_vector_get(&vec, 1);
    ASSERT(dg != NULL && *dg == 1.5, "vector insert middle - correct value");
    sct_vector_deinit(&vec);

    /* Bulk push to trigger reallocation */
    sct_vector_init(&vec, sizeof(int));
    for (int i = 0; i < 200; i++) {
        sct_vector_push(&vec, &i);
    }
    ASSERT(vec.size == 200, "vector bulk push - size == 200");
    int *ig = (int*)sct_vector_get(&vec, 199);
    ASSERT(ig != NULL && *ig == 199, "vector bulk push - last element correct");
    sct_vector_deinit(&vec);

    /* List: bulk push */
    sct_list_init(&list, sizeof(int));
    int count2 = 0;
    (void)count2;
    for (int i = 0; i < 100; i++) {
        sct_list_push(&list, &i);
    }
    ASSERT(list.size == 100, "list bulk push - size == 100");

    count = 0;
    foreach(&list) {
        int val = *(int*)((u8*)cur_pair + sizeof(void*));
        ASSERT(val == count, "list bulk push - foreach values correct");
        count++;
    }
    ASSERT(count == 100, "list bulk push - foreach visited all 100");
    sct_list_deinit(&list);

    /* List: erase from middle */
    sct_list_init(&list, sizeof(int));
    for (int i = 0; i < 10; i++) {
        sct_list_push(&list, &i);
    }
    sct_list_erase(&list, 4);
    ASSERT(list.size == 9, "list erase middle - size == 9");
    int *v2 = (int*)sct_list_get(&list, 4);
    ASSERT(v2 != NULL && *v2 == 5, "list erase middle - elements shifted");
    sct_list_deinit(&list);
}

int main(void)
{
    setvbuf(stdout, NULL, _IONBF, 0);
    printf("=== Simple Types Test Suite ===\n");

    test_arena();
    test_arena_vector();
    test_arena_list();
    test_arena_hashmap();
    test_vector();
    test_list();
    test_hashmap();
    test_string();
    test_vecslice();
    test_common();
    test_edge_cases();

    printf("\n=== Results: %d passed, %d failed ===\n",
           tests_passed, tests_failed);

    return tests_failed > 0 ? 1 : 0;
}