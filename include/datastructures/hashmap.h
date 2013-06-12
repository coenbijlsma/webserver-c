#ifndef CWS_HASHMAP_H
#define CWS_HASHMAP_H

#include <stdbool.h>
#include <stdint.h> // temp

typedef struct hashmap_bucket {
    void* value;
    struct hashmap_bucket* next;
} hashmap_bucket;

typedef struct hashmap_slot {
    hashmap_bucket* bucket;
    struct hashmap_slot* previous;
    struct hashmap_slot* next;
} hashmap_slot;

typedef struct hashmap {
    hashmap_slot* slot;
} hashmap;

uint32_t fnv1a_hash(const char* key);

/*
hashmap* hashmap_create(void);

void hashmap_destroy(hashmap* map);

void hashmap_put(hashmap* map, const char* key, const void* value);

void* hashmap_get(hashmap* map, const char* key);

void* hashmap_remove(hashmap* map, const char* key);

bool hashmap_contains_key(hashmap* map, const char* key);

bool hashmap_contains_value(hashmap* map, const void* value);
*/

#endif
