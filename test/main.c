#include <stdlib.h>
#include <string.h>
#include <check.h>

#include "util/hashmap.h"

void hashmap_put_test_keyvalue(hashmap* map)
{
    const char* key = "key";
    const void* value = (const void*)"value";
    hashmap_put(&map, key, value);
}

int str_comparator(const void* left, const void* right) {
    return strcmp((const char*)left, (const char*)right);
}

START_TEST(test_fnv1a_hash) 
{
    ck_assert(fnv1a_hash("foobar") == (uint32_t)875917790);
}
END_TEST

START_TEST(test_hashmap_create)
{
    hashmap* map = hashmap_create();
    ck_assert(map != (hashmap*)0);
    ck_assert(map->slot != (hashmap_slot*)0);
    ck_assert(map->slot->bucket != (hashmap_bucket*)0);

    // assume this works, is tested in another test
    hashmap_destroy(map);
}
END_TEST

START_TEST(test_hashmap_destroy)
{
    hashmap* map = hashmap_create();
    hashmap_destroy(map); // should not throw anything
}
END_TEST

START_TEST(test_hashmap_put)
{
    hashmap* map = hashmap_create();
    hashmap_put_test_keyvalue(map);
    ck_assert(map->size == 1);
    hashmap_destroy(map);
}
END_TEST

START_TEST(test_hashmap_get)
{
    hashmap* map = hashmap_create();
    hashmap_put_test_keyvalue(map);

    const char* char_value = (const char*)hashmap_get(map, "key");
    ck_assert_str_eq(char_value, "value");
    hashmap_destroy(map);
}
END_TEST

START_TEST(test_hashmap_contains_key)
{
    hashmap* map = hashmap_create();
    hashmap_put_test_keyvalue(map);

    ck_assert(hashmap_contains_key(map, "key"));
    hashmap_destroy(map);
}
END_TEST

START_TEST(test_hashmap_contains_value)
{
    hashmap* map = hashmap_create();
    const char* key = "key";
    const void* value = (const void*)"value";
    hashmap_put(&map, key, value);

    ck_assert(hashmap_contains_value(map, value));
    hashmap_destroy(map);
}
END_TEST

START_TEST(test_hashmap_contains_value_comp)
{
    hashmap* map = hashmap_create();
    hashmap_put_test_keyvalue(map);
   
    ck_assert(hashmap_contains_value_comp(map, str_comparator, (const void*)"value"));
    hashmap_destroy(map);
}
END_TEST

START_TEST(test_hashmap_remove)
{
    hashmap* map = hashmap_create();
    hashmap_put_test_keyvalue(map);

    const char* value = (const char*)hashmap_remove(map, "key");
    ck_assert_str_eq(value, "value");
    ck_assert(map->size == 0);
    hashmap_destroy(map);
}
END_TEST


Suite* ds_suite(void) 
{
    Suite* s = suite_create("test-datastructures");

    TCase* tc_core = tcase_create("core");
    tcase_add_test(tc_core, test_fnv1a_hash);
    tcase_add_test(tc_core, test_hashmap_create);
    tcase_add_test(tc_core, test_hashmap_destroy);
    tcase_add_test(tc_core, test_hashmap_put);
    tcase_add_test(tc_core, test_hashmap_get);
    tcase_add_test(tc_core, test_hashmap_contains_key);
    tcase_add_test(tc_core, test_hashmap_contains_value);
    tcase_add_test(tc_core, test_hashmap_contains_value_comp);
    tcase_add_test(tc_core, test_hashmap_remove);
    suite_add_tcase(s, tc_core);

    return s;
}

int main(void) 
{
    int number_failed;
    Suite* s = ds_suite();
    SRunner* sr = srunner_create(s);
    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
