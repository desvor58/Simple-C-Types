#include "../test_template.h"
#include "../../include/hashmap.h"

#include <stdarg.h>

char *format(char *fmt, ...)
{
    char *buf = malloc(4*1024);
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, 4*1024, fmt, args);
    va_end(args);
    char *res = malloc(strlen(buf) + 1);
    for (size_t i = 0; i < strlen(buf) + 1; i++) {
        res[i] = buf[i];
    }
    free(buf);
    return res;
}

int main(int argc, char **argv)
{
    test {
        log("map creating\n", 0);
        sct_hashmap_t *map = sct_hashmap_create();

        if (sct_list_size(map->keys) != 0) {
            err("Err:Wrong init keys_top\n"
                "    keys_top:%u\n",
                sct_list_size(map->keys));
        }

        for (size_t i = 0; i < 100; i++) {
            int *iBt = malloc(sizeof(int));
            *iBt = i;
            log("set 'iNum%u'\n", i);
            sct_hashmap_set(map, format("iNum%u", i), iBt);

            if (sct_list_size(map->keys) != i + 1) {
                err("Err:keys_top != i\n"
                    "    keys_top:%d\n"
                    "    i:%d\n",
                    sct_list_size(map->keys),
                    i);
            }

            size_t kt = sct_list_size(map->keys);
            for (size_t j = 0; j < kt; j++) {
                log("get 'iNum%u' expected %u\n", j, j);
                int *jBt = sct_hashmap_get(map, format("iNum%u", j));
                log("cmp jBt and j\n", 0);
                if (!jBt) {
                    err("Err:jBt == 0\n", 0);
                } else
                if (*jBt != j) {
                    err("Err:jBt != j\n"
                        "    *jBt:%d\n"
                        "    j:%d\n",
                        *jBt,
                        j);
                }
            }
        }

        sct_hashmap_full_free(map);
    } test_end;
}