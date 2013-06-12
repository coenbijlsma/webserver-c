#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "datastructures/hashmap.h"

#define FNV_PRIME 2147483647;

uint32_t get_offset_base() {
    uint32_t seed = 31;
    
    srand(seed);
    return (uint32_t)rand();
}

uint32_t fnv1a_hash(const char* key) {
    uint32_t hash = get_offset_base();
    for(int i = 0; i < strlen(key); i++) {
        hash ^= (unsigned char)key[i];
        hash *= FNV_PRIME;
        key++;
    }
    return hash;
}


