#include <SCT/arena_list.h>

void sct_arena_list_init(sct_arena_list_t *list, sct_arena_t *arena, size_t item_size)
{
    list->arena = arena;
    list->first_pair = NULL;
    list->last_pair = NULL;
    list->size = 0;
    list->_item_size = item_size;
}

void sct_arena_list_deinit(sct_arena_list_t *list)
{
    sct_arena_list_clear(list);
    list->arena = NULL;
    list->_item_size = 0;
}

void sct_arena_list_clear(sct_arena_list_t *list)
{
    list->first_pair = NULL;
    list->last_pair = NULL;
    list->size = 0;
}

void sct_arena_list_push(sct_arena_list_t *list, const void *item)
{
    u8 *new_pair;
    if (!list->first_pair) {
        list->first_pair = sct_arena_alloc(list->arena, sizeof(void*));
        *(void**)list->first_pair = NULL;
        list->last_pair = list->first_pair;
    }
    if (list->_item_size > SIZE_MAX - sizeof(void*)) {
        abort();
    }
    new_pair = sct_arena_alloc(list->arena, sizeof(void*) + list->_item_size);
    *(void**)new_pair = NULL;
    if (list->_item_size) {
        memcpy((u8*)new_pair + sizeof(void*), item, list->_item_size);
    }
    *(void**)list->last_pair = new_pair;
    list->last_pair = new_pair;
    list->size++;
}

void *sct_arena_list_get(const sct_arena_list_t *list, size_t index)
{
    u8 *cur_pair;
    if (index >= list->size) {
        return NULL;
    }
    cur_pair = *(void**)list->first_pair;
    while (index) {
        cur_pair = *(void**)cur_pair;
        index--;
    }
    return (u8*)cur_pair + sizeof(void*);
}

void sct_arena_list_erase(sct_arena_list_t *list, size_t index)
{
    u8 *prev_pair = list->first_pair;
    u8 *cur_pair;
    if (!list->size || index >= list->size) {
        return;
    }
    while (index) {
        prev_pair = *(void**)prev_pair;
        index--;
    }
    cur_pair = *(void**)prev_pair;
    *(void**)prev_pair = *(void**)cur_pair;
    if (list->last_pair == cur_pair) {
        list->last_pair = prev_pair;
    }
    list->size--;
}

size_t sct_arena_list_size(const sct_arena_list_t *list)
{
    return list ? list->size : 0;
}

void sct_arena_list_iter_init(sct_arena_list_iter_t *iter, sct_arena_list_t *list)
{
    iter->list = list;
    iter->previous = list->first_pair;
    iter->current = NULL;
    iter->next = list->first_pair ? *(void**)list->first_pair : NULL;
}

int sct_arena_list_iter_next(sct_arena_list_iter_t *iter)
{
    if (!iter->next) {
        iter->current = NULL;
        return 0;
    }
    if (iter->current) {
        iter->previous = iter->current;
    }
    iter->current = iter->next;
    iter->next = *(void**)iter->current;
    return 1;
}

void *sct_arena_list_iter_value(const sct_arena_list_iter_t *iter)
{
    return iter->current
        ? (u8*)iter->current + sizeof(void*)
        : NULL;
}

int sct_arena_list_iter_erase(sct_arena_list_iter_t *iter)
{
    void *next_pair;
    if (!iter->current) {
        return 1;
    }
    next_pair = iter->next;
    *(void**)iter->previous = next_pair;
    if (iter->list->last_pair == iter->current) {
        iter->list->last_pair = iter->previous;
    }
    iter->current = NULL;
    iter->next = next_pair;
    iter->list->size--;
    return 0;
}
