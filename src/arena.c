#include <arena.h>

void sct_arena_init(sct_arena_t *arena)
{
    arena->arena = amalloc(SCT_ARENA_ALLOC_SIZE);
    arena->size = 0;
    arena->cap = SCT_ARENA_ALLOC_SIZE;
}

void sct_arena_deinit(sct_arena_t *arena)
{
    free(arena->arena);
}

void *sct_arena_alloc(sct_arena_t *arena, size_t size)
{
    size_t aligned_size = sct_align_up(size);
    if (arena->size + aligned_size > arena->cap) {
        arena->cap += (aligned_size / SCT_ARENA_ALLOC_SIZE + 1) * SCT_ARENA_ALLOC_SIZE;
        void *new_arena = amalloc(arena->cap);
        memcpy(new_arena, arena->arena, arena->size);
        free(arena->arena);
        arena->arena = new_arena;
    }
    size_t ret_offset = arena->size;
    arena->size += aligned_size;
    return arena->arena + ret_offset;
}

void sct_arena_free(sct_arena_t *arena)
{
    arena->size = 0;
    if (arena->cap < SCT_ARENA_FREE_NONALLOC_LIMIT) {
        return;
    }
    free(arena->arena);
    arena->arena = amalloc(SCT_ARENA_ALLOC_SIZE);
    arena->cap = SCT_ARENA_ALLOC_SIZE;
}