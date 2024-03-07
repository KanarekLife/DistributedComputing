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
    printf("%ld", (long)n);
}

struct Point
{
    int x, y;
} point_t;

hash_t hash_point(data_t *point)
{
    struct Point *p = (struct Point *)point;
    return (hash_t)(p->x + p->y) % 10;
}

int compare_point(hash_t a, hash_t b)
{
    return a - b;
}

void print_point(data_t *point)
{
    struct Point *p = (struct Point *)point;
    printf("(%d, %d)", p->x, p->y);
}

void free_point(data_t *point) {
    free(point);
}

void do_nothin(data_t *point) {
    return;
}

int main()
{
    hashmap *map = hashmap_create(&hash, &compare, &print_int, &do_nothin);
    int data[] = {56, 32, 33, 88, 32, -32, 0, 0, 0, 11, -11, 100, 76, 22};
    for (long unsigned int i = 0; i < sizeof(data) / sizeof(int); i++)
    {
        hashmap_insert(map, (data_t *)data[i]);
    }
    hashmap_print(map);
    printf("get(0) with deletion: %ld\n", (unsigned long)hashmap_get(map, 0, true));
    printf("get(0) with deletion: %ld\n", (unsigned long)hashmap_get(map, 0, true));
    printf("get(0) with deletion: %ld\n", (unsigned long)hashmap_get(map, 0, true));
    printf("get(0) with deletion: %ld\n", (unsigned long)hashmap_get(map, 0, true));
    printf("get(2) with deletion: %ld\n", (unsigned long)hashmap_get(map, 2, true));
    printf("get(2) with deletion: %ld\n", (unsigned long)hashmap_get(map, 2, true));
    printf("get(8) with deletion: %ld\n", (unsigned long)hashmap_get(map, 8, true));
    printf("get(4) without deletion: %ld\n", (unsigned long)hashmap_get(map, 4, false));
    printf("get(4) with deletion: %ld\n", (unsigned long)hashmap_get(map, 4, true));
    printf("get(5) with deletion: %ld\n", (unsigned long)hashmap_get(map, 5, true));
    hashmap_print(map);
    hashmap_destroy(map);

    printf("\n\n");

    map = hashmap_create(&hash_point, &compare_point, &print_point, &do_nothin);
    struct Point points[] = {{1, 2}, {3, 4}, {5, 6}, {7, 8}, {9, 10}, {11, 12}, {13, 14}, {15, 16}, {17, 18}, {19, 20}};
    for (long unsigned int i = 0; i < sizeof(points) / sizeof(struct Point); i++)
    {
        hashmap_insert(map, (data_t *)&points[i]);
    }
    hashmap_print(map);
    printf("get(1) with deletion: %ld\n", (unsigned long)hashmap_get(map, 1, true));
    printf("get(1) with deletion: %ld\n", (unsigned long)hashmap_get(map, 1, true));
    printf("get(3) with deletion: %ld\n", (unsigned long)hashmap_get(map, 3, true));
    printf("get(5) with deletion: %ld\n", (unsigned long)hashmap_get(map, 5, true));
    printf("get(7) with deletion: %ld\n", (unsigned long)hashmap_get(map, 7, true));
    printf("get(9) with deletion: %ld\n", (unsigned long)hashmap_get(map, 9, true));
    hashmap_print(map);
    hashmap_destroy(map);

    printf("\n\n");

    map = hashmap_create(&hash_point, &compare_point, &print_point, &free_point);
    struct Point* pts[4];
    pts[0] = malloc(sizeof(struct Point));
    pts[0]->x = 5;
    pts[0]->y = 3;
    pts[1] = malloc(sizeof(struct Point));
    pts[1]->x = 3;
    pts[1]->y = 2;
    pts[2] = malloc(sizeof(struct Point));
    pts[2]->x = 9;
    pts[2]->y = 0;
    pts[3] = malloc(sizeof(struct Point));
    pts[3]->x = 9;
    pts[3]->y = 0;
    hashmap_insert(map, (data_t*)pts[0]);
    hashmap_insert(map, (data_t*)pts[1]);
    hashmap_insert(map, (data_t*)pts[2]);
    hashmap_insert(map, (data_t*)pts[3]);
    hashmap_print(map);
    hashmap_destroy(map);
}