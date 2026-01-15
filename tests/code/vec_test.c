#include "../test_template.h"
#include "../../include/SCT/vector.h"

int main(int argc, char **argv)
{
    test {
        log("vec init\n", 0);
        sct_vector_t *vec = sct_vector_create(5);
        if (vec->size) {
            err("Wrong init size\n",
                "    size:%u\n",
                "    aloc_size:%u\n",
                vec->size,
                vec->real_size);
        }

        size_t top = 50;
        for (size_t i = 0; i < top; i++) {
            int *it = malloc(sizeof(int));
            *it = i;
            log("vec adding i%u\n", i);
            sct_vector_push_back(vec, it);
            if (vec->size != i + 1) {
                err("Wrong vec size\n",
                    "    size:%u\n",
                    "    aloc_size:%u\n",
                    vec->size,
                    vec->real_size);
                break;
            }

            for (int j = 0; j < vec->size; j++) {
                log("vec get e%u\n", j);
                if (*((int*)vec->arr[j]) != j) {
                    err("Wrong element\n",
                        "    size:%u\n",
                        "    aloc_size:%u\n",
                        "    arr[j]:%d\n",
                        vec->size,
                        vec->real_size,
                        *((size_t*)vec->arr[j]));
                    break;
                }
            }
        }
        sct_vector_full_free(vec);
    } test_end;
}