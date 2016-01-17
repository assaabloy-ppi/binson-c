/*
 *	Test error handling routines
 */

#include "btest.h"

#include "binson/binson_error.h"

static void utest_binson_error(void **state) {
    (void) state;

    binson_io  *io;
    binson_res  res;
    uint8_t     cnt;
    int 	i,j=0;    

    UNUSED(i);   /* suppress warnings */
    UNUSED(j);    
    UNUSED(res);    
    
    res = binson_io_new(&io); 
    res = binson_io_open_file( io, "./utest_error.log", BINSON_IO_MODE_WRITE | BINSON_IO_MODE_CREATE );
    res = binson_error_init( io );
    
    if (FAILED(BINSON_RES_OK)) i=1;
    res = binson_error_dump(&cnt);
    assert_int_equal(cnt, 0);

    if (FAILED(BINSON_RES_TRAVERSAL_BREAK)) j=1;
    if (FAILED(BINSON_RES_TRAVERSAL_RESTART)) j=1;
    if (FAILED(BINSON_RES_TRAVERSAL_DONE)) j=1;
    if (FAILED(BINSON_RES_TRAVERSAL_CB)) j=1;
    res = binson_error_dump(&cnt);
    assert_int_equal(cnt, 4);
    res = binson_error_dump(&cnt);
    assert_int_equal(cnt, 0);    
    
    
    for (int i=1; i<222; i++)
    {
      if (FAILED(i)) j=1;
    }
    
    res = binson_error_dump(&cnt);
    assert_int_equal(cnt, ERROR_RING_SIZE);
    res = binson_error_dump(&cnt);
    assert_int_equal(cnt, 0);    
    
    
    res = binson_io_free(io);     
}

int utest_run_tests(void) {
  const struct CMUnitTest tests[] = {
	      cmocka_unit_test(utest_binson_error),
	      };
  return cmocka_run_group_tests(tests, NULL, NULL);
}