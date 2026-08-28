#include <SCT/list.h>

void sct_list_init(sct_list_t *list, size_t item_size)
{
    list->first_pair = amalloc(sizeof(void*) + item_size);
    *(void**)list->first_pair = 0;
    list->size = 0;
    list->_item_size = item_size;
}

static void sct_list_new_pair(sct_list_t *list, void *parent_pair, void *item)
{
    void *new_pair = amalloc(sizeof(void*) + list->_item_size);
    *(void**)new_pair = 0;
    memcpy((u8*)new_pair + sizeof(void*), item, list->_item_size);
    *(void**)parent_pair = new_pair;
}

void sct_list_deinit(sct_list_t *list)
{
    void *cur_pair = list->first_pair;
    void *next = *(void**)cur_pair;
    while (cur_pair) {
        next = *(void**)cur_pair;
        free(cur_pair);
        cur_pair = next;
    }
}

void sct_list_push(sct_list_t *list, void *item)
{
    void *cur_pair = list->first_pair;
    while (*(void**)cur_pair) {
        cur_pair = *(void**)cur_pair;
    }
    sct_list_new_pair(list, cur_pair, item);
    list->size++;
}

void *sct_list_get(sct_list_t *list, size_t index)
{
    if (index >= list->size || !list->size) return 0;
    size_t i = 0;
    void *cur_pair = *(void**)list->first_pair;
    while (i < index) {
        cur_pair = *(void**)cur_pair;
        i++;
    }
    return (u8*)cur_pair + sizeof(void*);
}

void sct_list_erase(sct_list_t *list, size_t index)
{
    if (index >= list->size || !list->size) return;

    void *prev_pair = list->first_pair;
    for (size_t i = 0; i < index; i++) {
        prev_pair = *(void**)prev_pair;
    }

    void *cur_pair = *(void**)prev_pair;
    *(void**)prev_pair = *(void**)cur_pair;
    free(cur_pair);
    list->size--;
}