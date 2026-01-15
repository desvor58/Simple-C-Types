#include "../test_template.h"
#include "../../include/SCT/list.h"
#include "../../include/SCT/vector.h"

int main(int argc, char **argv)
{
    test {
        log("list init\n", 0);
        sct_list_pair_t *list = sct_list_pair_create(0);
        if (sct_list_size(list)) {
            err("Err:Wrong list init size (!= 0)\n"
                    "    val:%u, next:%u\n",
                    list->val,
                    list->next);
        }

        size_t top = 100;
        sct_vector_t *indexes = sct_vector_create(200);
        for (size_t i = 0; i < top; i++) {
            int *it = malloc(sizeof(int));
            *it = i;
            int *index = malloc(sizeof(int));
            *index = i;
            sct_vector_push_back(indexes, index);

            log("list adding i%u\n", i);
            sct_list_push_back(list, it);
            if (sct_list_size(list) != i + 1) {
                err("Err:Wrong list size\n"
                    "    size:%u, next:%u\n",
                    sct_list_size(list),
                    list->next);
            }

            for (size_t j = 0; j < sct_list_size(list); j++) {
                log("list get e%u\n", j);
                int *el = sct_list_get(list, j);
                if (*el != j) {
                    err("Err:Wrong list element\n"
                        "    e:%d\n",
                        *el);
                }
            }
        }

        log("start of deleting test\n", 0);

        list = sct_list_full_delete(list, 0);
        sct_vector_delete(indexes, 0);

        for (size_t j = 0; j < sct_list_size(list); j++) {
            log("list get e%u\n", j);
            int *el = sct_list_get(list, j);
            int *index = indexes->arr[j];
            if (*el != *index) {
                err("Err:Wrong list element\n"
                    "    e:%d\n",
                    *el);
            }
        }

        list = sct_list_full_delete(list, 5);
        sct_vector_delete(indexes, 5);

        for (size_t j = 0; j < sct_list_size(list); j++) {
            log("list get e%u\n", j);
            int *el = sct_list_get(list, j);
            int *index = indexes->arr[j];
            if (*el != *index) {
                err("Err:Wrong list element\n"
                    "    e:%d\n",
                    *el);
            }
        }

        list = sct_list_full_delete(list, sct_list_size(list) - 1);
        sct_vector_delete(indexes, indexes->size - 1);

        for (size_t j = 0; j < sct_list_size(list); j++) {
            log("list get e%u\n", j);
            int *el = sct_list_get(list, j);
            int *index = indexes->arr[j];
            if (*el != *index) {
                err("Err:Wrong list element\n"
                    "    e:%d\n",
                    *el);
            }
        }

        sct_list_full_free(list);
    } test_end;
}