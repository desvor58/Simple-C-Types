#include <SCT/arena.h>

struct sct_arena_block {
    sct_arena_block_t *next;
    size_t used;
    size_t cap;
};

static sct_arena_block_t *sct_arena_block_new(size_t cap)
{
    sct_arena_block_t *block = amalloc(sizeof(sct_arena_block_t) + cap);
    block->next = NULL;
    block->used = 0;
    block->cap = cap;
    return block;
}

static u8 *sct_arena_block_data(sct_arena_block_t *block)
{
    return (u8*)block + sizeof(sct_arena_block_t);
}

void sct_arena_init(sct_arena_t *arena)
{
    arena->blocks = sct_arena_block_new(SCT_ARENA_ALLOC_SIZE);
    arena->cur = arena->blocks;
    arena->arena = sct_arena_block_data(arena->blocks);
    arena->size = 0;
    arena->cap = SCT_ARENA_ALLOC_SIZE;
}

void sct_arena_deinit(sct_arena_t *arena)
{
    sct_arena_block_t *block = arena->blocks;
    while (block) {
        sct_arena_block_t *next = block->next;
        free(block);
        block = next;
    }
    arena->blocks = NULL;
    arena->cur = NULL;
    arena->arena = NULL;
    arena->size = 0;
    arena->cap = 0;
}

void *sct_arena_alloc(sct_arena_t *arena, size_t size)
{
    size_t aligned_size = sct_align_up(size);
    sct_arena_block_t *block = arena->cur;
    if (block->used + aligned_size > block->cap) {
        size_t new_cap = aligned_size > SCT_ARENA_ALLOC_SIZE
            ? aligned_size
            : SCT_ARENA_ALLOC_SIZE;
        sct_arena_block_t *new_block = sct_arena_block_new(new_cap);
        block->next = new_block;
        arena->cur = new_block;
        arena->cap += new_cap;
        block = new_block;
    }
    size_t ret_offset = block->used;
    block->used += aligned_size;
    arena->size += aligned_size;
    return sct_arena_block_data(block) + ret_offset;
}

void sct_arena_free(sct_arena_t *arena)
{
    if (arena->cap < SCT_ARENA_FREE_NONALLOC_LIMIT) {
        for (sct_arena_block_t *block = arena->blocks; block; block = block->next) {
            block->used = 0;
        }
        arena->cur = arena->blocks;
        arena->size = 0;
        return;
    }
    sct_arena_deinit(arena);
    sct_arena_init(arena);
}
