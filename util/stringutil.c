#include <string.h>
#include <stddef.h>
#include "util/stringutil.h"

int strcmp_safe(const char* s1, const char* s2) {
    if(s1 == NULL && s2 != NULL) {
        return -1;
    } else if(s1 != NULL && s2 == NULL) {
        return 1;
    } else {
        return strcmp(s1, s2);
    }
}
