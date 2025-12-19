#include "../test_template.h"
#include "../../include/hashmap.h"

#include <stdarg.h>

genhashmap(int)

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
        hashmap_int_t *map = hashmap_int_create();

        if (map->keys_top != 0) {
            err("Err:Wrong init keys_top\n"
                "    keys_top:%u\n",
                map->keys_top);
        }

        for (size_t i = 0; i < 100; i++) {
            int *iBt = malloc(sizeof(int));
            *iBt = i;
            log("set 'iNum%u'\n", i);
            hashmap_int_set(map, format("iNum%u", i), iBt);

            if (map->keys_top - 1 != i) {
                err("Err:keys_top != i\n"
                    "    keys_top:%d\n"
                    "    i:%d\n",
                    map->keys_top,
                    i);
            }

            size_t kt = map->keys_top;
            for (size_t j = 0; j < kt; j++) {
                log("get 'iNum%u' expected %u\n", j, j);
                int *jBt = hashmap_int_get(map, format("iNum%u", j));
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

        hashmap_int_free(map);
    } test_end;
}