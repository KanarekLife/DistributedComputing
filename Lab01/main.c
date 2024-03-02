#include "hashmap.h"

hash_t hash(data_t *n)
{
    return (hash_t)n % 10;
}

int compare(hash_t a, hash_t b)
{
    return a - b;
}

void print_int(data_t *n)
{
    printf("%d", (int)n);
}

int main()
{
    hashmap *map = hashmap_create(&hash, &compare, &print_int);
    int data[] = {56, 32, 33, 88, 32, -32, 0, 0, 0, 11, -11, 100, 76, 22};
    for (long unsigned int i = 0; i < sizeof(data) / sizeof(int); i++)
    {
        hashmap_insert(map, (data_t *)data[i]);
    }
    hashmap_print(map);
    printf("get(0) with deletion: %d\n", (unsigned int)hashmap_get(map, 0, true));
    printf("get(2) with deletion: %d\n", (unsigned int)hashmap_get(map, 2, true));
    printf("get(2) with deletion: %d\n", (unsigned int)hashmap_get(map, 2, true));
    printf("get(8) with deletion: %d\n", (unsigned int)hashmap_get(map, 8, true));
    printf("get(4) without deletion: %d\n", (unsigned int)hashmap_get(map, 4, true));
    printf("get(5) with deletion: %d\n", (unsigned int)hashmap_get(map, 5, true));
    hashmap_destroy(map);
}