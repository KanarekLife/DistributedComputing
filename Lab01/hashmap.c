#include "hashmap.h"

hash_node *hashmap_find(hashmap *map, hash_t hash)
{
    hash_node *node = map->hash_nodes;
    while (node != NULL)
    {
        if (map->compare(node->hash, hash) == 0)
        {
            return node;
        }
        node = node->next;
    }
    return NULL;
}

hashmap *hashmap_create(hash_t (*hash)(data_t *), int (*compare)(hash_t, hash_t), void (*print)(data_t *))
{
    hashmap *map = malloc(sizeof(hashmap));
    if (map == NULL)
    {
        return NULL;
    }
    map->hash = hash;
    map->compare = compare;
    map->print = print;
    map->hash_nodes = NULL;
    return map;
}

void hashmap_destroy(hashmap *map)
{
    hash_node *node = map->hash_nodes;
    while (node != NULL)
    {
        hash_node *next = node->next;
        data_node *data = node->data_nodes;
        while (data != NULL)
        {
            data_node *next = data->next;
            free(data);
            data = next;
        }
        free(node);
        node = next;
    }
    free(map);
}

bool hashmap_insert(hashmap *map, data_t *data)
{
    hash_t hash = map->hash(data);
    hash_node *node = hashmap_find(map, hash);
    if (node == NULL)
    {
        node = malloc(sizeof(hash_node));
        if (node == NULL)
        {
            return false;
        }
        node->hash = hash;
        node->data_nodes = NULL;
        node->prev = NULL; // Assign prev as NULL

        // Find the correct position to insert the node in sorted order
        hash_node *current = map->hash_nodes;
        hash_node *prev = NULL;
        while (current != NULL && map->compare(current->hash, hash) < 0)
        {
            prev = current;
            current = current->next;
        }

        // Insert the node at the correct position
        if (prev != NULL)
        {
            prev->next = node;
            node->prev = prev; // Assign prev to the previous node
        }
        else
        {
            map->hash_nodes = node;
        }
        node->next = current;
        if (current != NULL)
        {
            current->prev = node; // Assign prev to the next node
        }
    }

    data_node *new_data = malloc(sizeof(data_node));
    if (new_data == NULL)
    {
        return false;
    }
    new_data->data = data;
    new_data->prev = NULL;
    new_data->next = NULL;

    // Find the correct position to insert the data in sorted order
    data_node *current = node->data_nodes;
    data_node *prev = NULL;
    while (current != NULL && map->compare(map->hash(current->data), hash) < 0)
    {
        prev = current;
        current = current->next;
    }

    // Insert the data at the correct position
    if (prev != NULL)
    {
        prev->next = new_data;
    }
    else
    {
        node->data_nodes = new_data;
    }
    new_data->prev = prev;
    new_data->next = current;
    if (current != NULL)
    {
        current->prev = new_data;
    }

    return true;
}

data_t *hashmap_get(hashmap *map, hash_t hash, bool shouldDeleteAfterRetrieval)
{
    hash_node *node = hashmap_find(map, hash);
    if (node == NULL)
    {
        return NULL;
    }
    data_node *data = node->data_nodes;
    while (data != NULL)
    {
        if (map->compare(map->hash(data->data), hash) == 0)
        {
            data_t *ret = data->data;
            if (shouldDeleteAfterRetrieval)
            {
                if (data->prev != NULL)
                {
                    data->prev->next = data->next;
                }
                else
                {
                    node->data_nodes = data->next;
                }
                if (data->next != NULL)
                {
                    data->next->prev = data->prev;
                }
                free(data);

                // Delete empty hash_node if no data_t remains
                if (node->data_nodes == NULL)
                {
                    if (node->prev != NULL)
                    {
                        node->prev->next = node->next;
                    }
                    else
                    {
                        map->hash_nodes = node->next;
                    }
                    if (node->next != NULL)
                    {
                        node->next->prev = node->prev;
                    }
                    free(node);
                }
            }
            return ret;
        }
        data = data->next;
    }
    return NULL;
}

void hashmap_print(hashmap *map)
{
    hash_node *node = map->hash_nodes;
    while (node != NULL)
    {
        printf("Hash: %ld\n", node->hash);
        data_node *data = node->data_nodes;
        while (data != NULL)
        {
            map->print(data->data);
            printf(" ");
            data = data->next;
        }
        printf("\n");
        node = node->next;
    }
}
