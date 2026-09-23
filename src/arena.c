#include <SCT/arena.h>

struct sct_arena_block {
    sct_arena_block_t *next;
    u8                *allocation;
    u8                *data;
    size_t             used;
    size_t             cap;
    size_t             alignment;
};

static sct_arena_block_t *sct_arena_block_new(size_t cap, size_t alignment)
{
    size_t total;
    size_t offset = 0;
    u8 *data;
    sct_arena_block_t *block;

    if (!alignment || (alignment & (alignment - 1)) ||
        cap > SIZE_MAX - sizeof(sct_arena_block_t) - (alignment - 1)) {
        abort();
    }
    total = sizeof(sct_arena_block_t) + alignment - 1 + cap;
    block = amalloc(total);
    block->next = NULL;
    block->allocation = (u8*)block;
    data = block->allocation + sizeof(sct_arena_block_t);
    while ((uintptr_t)(data + offset) & (alignment - 1)) {
        offset++;
    }
    block->data = data + offset;
    block->used = 0;
    block->cap = cap;
    block->alignment = alignment;
    return block;
}

void sct_arena_init(sct_arena_t *arena)
{
    arena->alloc_size = SCT_ARENA_DEFAULT_ALIGNMENT;
    arena->blocks = sct_arena_block_new(SCT_ARENA_ALLOC_SIZE, arena->alloc_size);
    arena->cur = arena->blocks;
    arena->arena = arena->cur->data;
    arena->size = 0;
    arena->cap = SCT_ARENA_ALLOC_SIZE;
}

void sct_arena_deinit(sct_arena_t *arena)
{
    sct_arena_block_t *block = arena->blocks;
    while (block) {
        sct_arena_block_t *next = block->next;
        free(block->allocation);
        block = next;
    }
    arena->blocks = NULL;
    arena->cur = NULL;
    arena->arena = NULL;
    arena->size = 0;
    arena->cap = 0;
    arena->alloc_size = 0;
}

void *sct_arena_alloc_aligned(sct_arena_t *arena, size_t size, size_t alignment)
{
    size_t aligned_size;
    size_t aligned_used;
    sct_arena_block_t *block;
    u8 *ret;

    if (!arena || !arena->blocks || !alignment || (alignment & (alignment - 1))) {
        abort();
    }
    if (sct_align_up_checked(size, alignment, &aligned_size)) {
        abort();
    }
    block = arena->cur;
    if (!size) {
        arena->arena = block->data + block->used;
        return arena->arena;
    }
    if (block->alignment < alignment) {
        goto new_block;
    }
    if (sct_align_up_checked(block->used, alignment, &aligned_used)) {
        goto new_block;
    }
    if (aligned_used <= block->cap && size <= block->cap - aligned_used) {
        goto use_block;
    }

new_block:
    if (alignment > arena->alloc_size) {
        arena->alloc_size = alignment;
    }
    {
        size_t new_cap = aligned_size > SCT_ARENA_ALLOC_SIZE
            ? aligned_size
            : SCT_ARENA_ALLOC_SIZE;
        sct_arena_block_t *new_block;
        if (arena->cap > SIZE_MAX - new_cap) {
            abort();
        }
        new_block = sct_arena_block_new(new_cap, arena->alloc_size);
        block->next = new_block;
        arena->cur = new_block;
        arena->cap += new_cap;
        block = new_block;
    }
    aligned_used = 0;

use_block:
    ret = block->data + aligned_used;
    block->used = aligned_used + aligned_size;
    arena->size += aligned_size;
    arena->arena = ret;
    return ret;
}

void *sct_arena_alloc(sct_arena_t *arena, size_t size)
{
    return sct_arena_alloc_aligned(arena, size, SCT_ARENA_DEFAULT_ALIGNMENT);
}

void *sct_arena_alloc_zero(sct_arena_t *arena, size_t size)
{
    void *ptr = sct_arena_alloc(arena, size);
    if (size) {
        memset(ptr, 0, size);
    }
    return ptr;
}

void *sct_arena_realloc(sct_arena_t *arena, void *ptr, size_t old_size, size_t new_size)
{
    void *new_ptr;
    size_t copy_size;

    if (!ptr) {
        return sct_arena_alloc(arena, new_size);
    }
    if (!new_size) {
        return NULL;
    }
    new_ptr = sct_arena_alloc(arena, new_size);
    copy_size = old_size < new_size ? old_size : new_size;
    if (copy_size) {
        memcpy(new_ptr, ptr, copy_size);
    }
    return new_ptr;
}

void sct_arena_free(sct_arena_t *arena)
{
    if (arena->cap < SCT_ARENA_FREE_NONALLOC_LIMIT) {
        sct_arena_block_t *block;
        for (block = arena->blocks; block; block = block->next) {
            block->used = 0;
        }
        arena->cur = arena->blocks;
        arena->size = 0;
        arena->arena = arena->cur->data;
        return;
    }
    sct_arena_deinit(arena);
    sct_arena_init(arena);
}
