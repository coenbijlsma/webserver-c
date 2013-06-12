#include <stdlib.h>
#include <check.h>

#include "datastructures/hashmap.h"

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

Suite* ds_suite(void) 
{
    Suite* s = suite_create("test-datastructures");

    TCase* tc_core = tcase_create("core");
    tcase_add_test(tc_core, test_fnv1a_hash);
    tcase_add_test(tc_core, test_hashmap_create);
    tcase_add_test(tc_core, test_hashmap_destroy);
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
