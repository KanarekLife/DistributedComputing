#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

typedef unsigned long hash_t;
typedef void data_t; // now data referenced in hashmap will be void* for data_t*

typedef struct data_node_t
{
    data_t *data;

    struct data_node_t *prev, *next;
} data_node;

typedef struct hashmap_node_t
{
    hash_t hash;
    data_node *data_nodes;

    struct hashmap_node_t *prev, *next;
} hash_node;

typedef struct hashmap_t
{
    hash_node *hash_nodes;
    hash_t (*hash)(data_t *);
    int (*compare)(hash_t, hash_t);
    void (*print)(data_t *);
} hashmap;

hashmap *hashmap_create(hash_t (*hash)(data_t *), int (*compare)(hash_t, hash_t), void (*print)(data_t *));
void hashmap_destroy(hashmap *map);
bool hashmap_insert(hashmap *map, data_t *data);
data_t *hashmap_get(hashmap *map, hash_t hash, bool shouldDeleteAfterRetrieval);
void hashmap_print(hashmap *map);