#ifndef SCT_ARENA_LIST_H
#define SCT_ARENA_LIST_H

#include "common.h"
#include "arena.h"

#define sct_arena_get_val(pair, T) *(T*)((u8*)pair + sizeof(void*))

#define sct_arena_foreach(list)  \
    for (void *cur_pair = (list)->first_pair ? *(void**)(list)->first_pair : NULL, *next_pair = cur_pair ? *(void**)cur_pair : NULL;  \
         cur_pair;  \
         cur_pair = next_pair, next_pair = cur_pair ? *(void**)cur_pair : NULL)

typedef struct {
    u8           *first_pair;
    size_t        size;
    size_t        _item_size;
    sct_arena_t  *arena;
} sct_arena_list_t;

void sct_arena_list_init(sct_arena_list_t *list, sct_arena_t *arena, size_t item_size);

void sct_arena_list_deinit(sct_arena_list_t *list);

void sct_arena_list_push(sct_arena_list_t *list, void *item);

void *sct_arena_list_get(sct_arena_list_t *list, size_t index);

void sct_arena_list_erase(sct_arena_list_t *list, size_t index);

#endif