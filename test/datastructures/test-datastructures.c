#include <check.h>

#include "datastructures/hashmap.h"

START_TEST(test_fnv1a_hash) {
    return fnv1a_hash("foobar") == (uint32_t)875917790;
}
END_TEST

