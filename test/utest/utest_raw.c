/*
 *	Test basic types <-> raw conversion routines
 */

#include "btest.h"

static void utest_some(void **state) {
    (void) state;

    assert_int_equal(1, 1);
}

int utest_run_tests(void) {
  const struct CMUnitTest tests[] = {
	      cmocka_unit_test(utest_some),
	      };
  return cmocka_run_group_tests(tests, NULL, NULL);
}