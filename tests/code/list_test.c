#include "../test_template.h"
#include "../../include/list.h"

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

        size_t top = 50;
        for (size_t i = 0; i < top; i++) {
            int *it = malloc(sizeof(int));
            *it = i;
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
                        "    e:%u\n",
                        *el);
                }
            }
        }
        sct_list_full_free(list);
    } test_end;
}