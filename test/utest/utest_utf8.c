/*
 *	Unit tests for 'binson_utf8' module
 */
/*#include <math.h>
#include <float.h>
*/

#include "btest.h"

#include "binson_utf8.h"


static void utest_binson_utf8(void **state) {

    (void) state;
    //binson_utf8_is_valid
    
    assert_true(1);
}


int utest_run_tests(void) {
  const struct CMUnitTest tests[] = {
	      cmocka_unit_test(utest_binson_utf8),
	      };
  return cmocka_run_group_tests(tests, NULL, NULL);
}