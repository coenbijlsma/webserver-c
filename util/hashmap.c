#include <stdint.h>
#include <stdlib.h>
#include <stddef.h> // true, false
#include <string.h> // memset, string fnc
#include <limits.h>

#include "util/stringutil.h"
#include "util/hashmap.h"

#define FNV_PRIME 2147483647
#define SEED 31

/***********************************
 *        PUBLIC FUNCTIONS
 **********************************/

static hashmap_slot* hashmap_retrieve_slot(hashmap* map, uint32_t index);
static void hashmap_do_insert(hashmap* map, hashmap_slot* slot, const char* key, const void* value);
static void hashmap_grow(hashmap** map, uint32_t new_capacity);
static uint32_t get_offset_base(void);
static hashmap_slot* hashmap_slot_create(void);
static hashmap_bucket* hashmap_bucket_create(void);
static void hashmap_slots_destroy(hashmap_slot* slot);
static void hashmap_buckets_destroy(hashmap_bucket* bucket);
static void hashmap_bucket_destroy(hashmap_bucket* bucket);

uint32_t fnv1a_hash(const char* key) {
    uint32_t hash = get_offset_base();
    for(int i = 0; i < strlen(key); i++) {
        hash ^= (unsigned char)key[i];
        hash *= FNV_PRIME;
        key++;
    }
    return hash;
}

hashmap* hashmap_create(void) {
    hashmap* map = (hashmap*)malloc(sizeof(hashmap));
    memset((void*)map, 0, sizeof(hashmap));

   map->slot = hashmap_slot_create(); 
   map->bucket_count = 1;
   map->slot_count = 1;
   return map;
}

void hashmap_destroy(hashmap* map) {
    if(map == (hashmap*)0) {
        return;
    }

    hashmap_slots_destroy(map->slot);
    free(map);
}

bool hashmap_contains_key(hashmap* map, const char* key) {
    return hashmap_get(map, key) != (void*)0;
}

bool hashmap_contains_value(hashmap* map, const void* value) {
    if(map == (hashmap*)0) {
        return false;
    }

    hashmap_slot* slot = map->slot;
    while(slot) {
        hashmap_bucket* bucket = slot->bucket;

        while(bucket) {
            if(bucket->value == value) {
                return true;
            }
            bucket = bucket->next;
        }
        slot = slot->next;
    }
    return false;
}

bool hashmap_contains_value_comp(hashmap* map, int (*comparator)(const void*, const void*),
        const void* value) {
    if(map == (hashmap*)0 || comparator == 0 || value == (const void*)0 ) {
        return false;
    }

    hashmap_slot* slot = map->slot;
    while(slot) {
        hashmap_bucket* bucket = slot->bucket;

        while(bucket) {
            if(bucket->value) { // only compare non-NULL values
                if( (*comparator)(bucket->value, value) == 0) {
                    return true;
                }
            }
            bucket = bucket->next;
        }
        slot = slot->next;
    }
    return false;
}

const void* hashmap_get(hashmap* map, const char* key) {
    if(map == (hashmap*)0 || key == (const char*)0) {
        return (void*)0;
    }
    
    uint32_t hash = fnv1a_hash(key);
    hashmap_slot* slot = hashmap_retrieve_slot(map, hash % map->size);

    if(slot != (hashmap_slot*)0) {
        hashmap_bucket* bucket = slot->bucket;
        while(bucket) {
            if(strcmp_safe(bucket->key, key) == 0) {
                return (const void*)bucket->value;
            }
            bucket = bucket->next;
        }
    }
    return (const void*)0;
}

void hashmap_put(hashmap** map, const char* key, const void* value) {
    float load_factor = ((*map)->size * 1.0f)/(*map)->bucket_count;

    if(load_factor >= 0.74f && load_factor <= 0.76f) {
        if( ((*map)->bucket_count  * 2) < UINT_MAX) {
            hashmap_grow(map, (uint32_t)((*map)->slot_count * 2)); // buckets are added automatically
        } else {
            // output warning smth like nearing map size limit
        }
    }

    uint32_t hash = fnv1a_hash(key);
    uint32_t divisor = (*map)->size == 0 ? hash : (*map)->size;
    hashmap_slot* slot = hashmap_retrieve_slot(*map, hash % divisor);
    hashmap_do_insert(*map, slot, key, value);
}

const void* hashmap_remove(hashmap* map, const char* key) {
    if(key == NULL) {
        return NULL;
    }

    uint32_t hash = fnv1a_hash(key);
    hashmap_slot* slot = hashmap_retrieve_slot(map, hash % map->slot_count);

    if(slot) {
        hashmap_bucket* bucket = slot->bucket;

        while(bucket) {
            if(strcmp_safe(bucket->key, key) == 0) {
                // XXX This causes fragmentation and slows down performance
                // over time. Have to fix this some time.
                const void* value = bucket->value;
                bucket->key = (const char*)NULL;
                bucket->value = NULL;
                map->size -= 1;
                return value;
            }

            bucket = bucket->next;
        }
    }
    return NULL;
}

/***********************************
 *        PRIVATE FUNCTIONS
 **********************************/

static void hashmap_do_insert(hashmap* map, hashmap_slot* slot, const char* key
        , const void* value) {
    hashmap_bucket* bucket = slot->bucket;

    while(bucket->value != (const void*)0 && bucket->next != (hashmap_bucket*)0) {
        bucket = bucket->next;
    }

    if(bucket->next == (hashmap_bucket*)0) {
        bucket->next = hashmap_bucket_create();
        bucket = bucket->next;
        map->bucket_count += 1;
    }

    bucket->key = key;
    bucket->value = value;
    map->size += 1;
}

static void hashmap_grow(hashmap** map, uint32_t new_capacity) {
    hashmap* new_map = hashmap_create();
    hashmap_slot* slot = new_map->slot;

    for(uint32_t i = 1; i < new_capacity; i++) { // already 1 slot present
        slot->next = hashmap_slot_create();
        slot = slot->next;
    }
    new_map->bucket_count = new_capacity;
    new_map->slot_count = new_capacity;

    slot = (*map)->slot;
    while(slot) {
        hashmap_bucket* bucket = slot->bucket;

        while(bucket) {
            uint32_t hash = fnv1a_hash(bucket->key);
            hashmap_slot* insert_into = hashmap_retrieve_slot(new_map
                    , hash % new_map->slot_count);
            hashmap_do_insert(new_map, insert_into, bucket->key, bucket->value);

            bucket = bucket->next;
        }
        slot = slot->next;
    }
    map = &new_map;
}

static hashmap_slot* hashmap_retrieve_slot(hashmap* map, uint32_t index) {
    hashmap_slot* slot = map->slot;

    for(uint32_t i = 0; i < index; i++) {
        if(slot == (hashmap_slot*)0) {
            break; // none found
        }
        slot = slot->next;
    }

    return slot;
}

static uint32_t get_offset_base(void) {
    srand(SEED);
    return (uint32_t)rand();
}

static hashmap_slot* hashmap_slot_create(void) {
    hashmap_slot* slot = (hashmap_slot*)malloc(sizeof(hashmap_slot));
    memset((void*)slot, 0, sizeof(hashmap_slot));

    slot->bucket = hashmap_bucket_create();
    return slot;
}

static hashmap_bucket* hashmap_bucket_create(void) {
    hashmap_bucket* bucket = (hashmap_bucket*)malloc(sizeof(hashmap_bucket));
    memset((void*)bucket, 0, sizeof(hashmap_bucket));

    return bucket;
}

static void hashmap_slots_destroy(hashmap_slot* slot) {
    if(slot == (hashmap_slot*)0) {
        return;
    }

    while(slot) {
        hashmap_buckets_destroy(slot->bucket);
        hashmap_slot* next = slot->next;
        free(slot);
        slot = next;
    }
}

static void hashmap_buckets_destroy(hashmap_bucket* bucket) {
    if(bucket == (hashmap_bucket*)0) {
        return;
    }

    while(bucket) {
        hashmap_bucket* next = bucket->next;
        hashmap_bucket_destroy(bucket);
        bucket = next;
    }
}

static void hashmap_bucket_destroy(hashmap_bucket* bucket) {
    if(bucket == NULL) {
        return;
    }

    free(bucket);
}
