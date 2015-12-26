#include "btest.h"

static void utest_some(void **state) {
    int rc;
    (void) state;

    rc = 0;
    assert_int_equal(rc, 1);
}

int utest_run_tests(void) {
  const struct CMUnitTest tests[] = {
            cmocka_unit_test(utest_some),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
