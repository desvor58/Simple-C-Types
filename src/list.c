#include <list.h>

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
    if (list->size == 0) {
        memcpy((u8*)list->first_pair + sizeof(void*), item, list->_item_size);
        list->size++;
        return;
    }
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
    void *cur_pair = list->first_pair;
    while (i < index) {
        cur_pair = *(void**)cur_pair;
        i++;
    }
    return (u8*)cur_pair + sizeof(void*);
}

void sct_list_erase(sct_list_t *list, size_t index)
{
    if (index >= list->size || !list->size) return;
    if (index == 0) {
        if (list->size == 1) {
            *(void**)list->first_pair = 0;
        } else {
            void *second_pair = *(void**)list->first_pair;
            void *third_pair = *(void**)second_pair;
            
            memcpy(list->first_pair + sizeof(void*), (u8*)second_pair + sizeof(void*), list->_item_size);
            
            *(void**)list->first_pair = third_pair;
            
            free(second_pair);
        }
        list->size--;
        return;
    }
    size_t i = 0;
    void *prev_pair = list->first_pair;
    while (i < index - 1) {
        prev_pair = *(void**)prev_pair;
        i++;
    }

    void *cur_pair = *(void**)prev_pair;

    if (!*(void**)cur_pair) {
        *(void**)prev_pair = 0;
    } else {
        *(void**)prev_pair = *(void**)cur_pair;
    }
    free(cur_pair);
    list->size--;
}