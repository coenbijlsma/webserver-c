#include "net/inet.h"

uint64_t swap(uint64_t source);

uint64_t htonll(uint64_t source) {
#if BYTE_ORDER == LITTLE_ENDIAN
    return swap(source);
#else
    return source;
#endif
}

uint64_t ntohll(uint64_t source) {
#if BYTE_ORDER == LITTLE_ENDIAN
    return swap(source);
#else
    return source;
#endif
}

uint64_t swap(uint64_t source) {
    uint64_t result = 0;
    uint64_t mask = 0xff;

    for(int i = 7; i >= -7; i -= 2) {
        if(i >= 0) {
            result |= ( (source & mask) << (i * 8) );
        } else {
            result |= ( (source & mask) >> (-i * 8) );
        }
        mask = (mask << 8);
    }
    return result;
}

