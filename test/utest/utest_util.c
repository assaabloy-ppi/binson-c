/*
 *	Unit tests for 'binson_util' module
 */
#include <math.h>
#include <float.h>

#include "btest.h"

#include "binson_util.h"

const uint8_t sample1[] = { 0x0 };
const uint8_t sample2[] = { 0x37 };
const uint8_t sample3[] = { 0xc9 };
const uint8_t sample4[] = { 0x80, 0x00 };
const uint8_t sample5[] = { 0x80 };    
const uint8_t sample6[] = { 0xd4, 0x00 };
const uint8_t sample7[] = { 0x2c, 0xff };    
const uint8_t sample8[] = { 0x39, 0x30 };    
const uint8_t sample9[] = { 0xc7, 0xcf };    
const uint8_t sample10[] = { 0xFE, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00 };    
const uint8_t sample11[] = { 0x02, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF };    
const uint8_t sample12[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F };    
const uint8_t sample13[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80 };    

const uint8_t dsample1[] = { 0xfc, 0x17, 0xac, 0xd2, 0x95, 0x96, 0xf5, 0xbd };
const uint8_t dsample2[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x7f };
const uint8_t dsample3[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x7f };    
const uint8_t dsample4[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xff };        


static void utest_binson_util_pack_integer(void **state) {

    (void) state;
    size_t  result;
    uint8_t buf[10];       
    
    /* case: zero */
    result = binson_util_pack_integer( 0, buf );
    assert_int_equal(result, 1);
    assert_memory_equal(buf, sample1, result );
    
    /* case: short positive */
    result = binson_util_pack_integer( 55, buf );
    assert_int_equal(result, 1);
    assert_memory_equal(buf, sample2, result );

    /* case: short negative */
    result = binson_util_pack_integer( -55, buf );
    assert_int_equal(result, 1);
    assert_memory_equal(buf, sample3, result );

    /* case: short positive */
    result = binson_util_pack_integer( 128, buf );
    assert_int_equal(result, 2);
    assert_memory_equal(buf, sample4, result );

    /* case: short negative */
    result = binson_util_pack_integer( -128, buf );
    assert_int_equal(result, 1);
    assert_memory_equal(buf, sample5, result );
    
    
    /* case: multibyte short positive */
    result = binson_util_pack_integer( 212, buf );
    assert_int_equal(result, 2);
    assert_memory_equal(buf, sample6, result );

    /* case: multibyte short negative */
    result = binson_util_pack_integer( -212, buf );
    assert_int_equal(result, 2);
    assert_memory_equal(buf, sample7, result );

    /* case: multibyte int positive */
    result = binson_util_pack_integer( 12345, buf );
    assert_int_equal(result, 2);
    assert_memory_equal(buf, sample8, result );

    /* case: another multibyte int negative */
    result = binson_util_pack_integer( -12345, buf );
    assert_int_equal(result, 2);
    assert_memory_equal(buf, sample9, result );

    /* case: INT32_MAX * 2 */
    result = binson_util_pack_integer( 4294967294L, buf );
    assert_int_equal(result, 8);
    assert_memory_equal(buf, sample10, result );
    
    /* case: -INT32_MAX * 2 */
    result = binson_util_pack_integer( -4294967294L, buf );
    assert_int_equal(result, 8);
    assert_memory_equal(buf, sample11, result );

    /* case: INT64_MAX */
    result = binson_util_pack_integer( INT64_MAX, buf );
    assert_int_equal(result, 8);
    assert_memory_equal(buf, sample12, result );
    
    /* case: INT64_MIN */
    result = binson_util_pack_integer( INT64_MIN, buf );
    assert_int_equal(result, 8);
    assert_memory_equal(buf, sample13, result );       
}

static void utest_binson_util_pack_double(void **state) {

    (void) state;
    size_t  result;
    uint8_t buf[10];
        
    /* case:  */
    result = binson_util_pack_double( -3.1415e-10, buf );
    assert_int_equal(result, 8);
    assert_memory_equal(buf, dsample1, result );
    
    /* case:  */
    result = binson_util_pack_double( NAN, buf );
    assert_int_equal(result, 8);
    assert_memory_equal(buf, dsample2, result );

    /* case: +Infinity */
    result = binson_util_pack_double( INFINITY, buf );
    assert_int_equal(result, 8);
    assert_memory_equal(buf, dsample3, result );
    
    /* case: -Infinity */
    result = binson_util_pack_double( -INFINITY, buf );
    assert_int_equal(result, 8);
    assert_memory_equal(buf, dsample4, result );    
}

static void utest_binson_util_unpack_integer(void **state) {
    (void) state;
        
    /* case: zero */
    assert_int_equal(binson_util_unpack_integer( sample1, 1 ), 0);
    
    /* case: short positive */
    assert_int_equal(binson_util_unpack_integer( sample2, 1 ), 55);

    /* case: short negative */
    assert_int_equal(binson_util_unpack_integer( sample3, 1 ), -55);

    /* case: short positive */
    assert_int_equal(binson_util_unpack_integer( sample4, 2 ), 128);
    
    /* case: short negative */
    assert_int_equal(binson_util_unpack_integer( sample5, 1 ), -128);
    
    /* case: multibyte short positive */
    assert_int_equal(binson_util_unpack_integer( sample6, 2 ), 212);

    /* case: multibyte short negative */
    assert_int_equal(binson_util_unpack_integer( sample7, 2 ), -212);

    /* case: multibyte int positive */
    assert_int_equal(binson_util_unpack_integer( sample8, 2 ), 12345);

    /* case: another multibyte int negative */
    assert_int_equal(binson_util_unpack_integer( sample9, 2 ), -12345);

    /* case: INT32_MAX * 2 */
    assert_int_equal(binson_util_unpack_integer( sample10, 8 ), 4294967294L);
    
    /* case: -INT32_MAX * 2 */
    assert_int_equal(binson_util_unpack_integer( sample11, 8 ), -4294967294L);

    /* case: INT64_MAX */
    assert_int_equal(binson_util_unpack_integer( sample12, 8 ), INT64_MAX);
    
    /* case: INT64_MIN */
    assert_int_equal(binson_util_unpack_integer( sample13, 8 ), INT64_MIN);
}

static void utest_binson_util_unpack_double(void **state) {
    (void) state;   
    
    assert_true( binson_util_unpack_double(dsample1) == -3.1415e-10 );
    assert_true( isnan(binson_util_unpack_double(dsample2)) );    	 /* (NAN == NAN) is false so use isnan() to test  */
    assert_true( binson_util_unpack_double(dsample3) == INFINITY );
    assert_true( binson_util_unpack_double(dsample4) == -INFINITY );
}

int utest_run_tests(void) {
  const struct CMUnitTest tests[] = {
	      cmocka_unit_test(utest_binson_util_pack_integer),
	      cmocka_unit_test(utest_binson_util_pack_double),
	      cmocka_unit_test(utest_binson_util_unpack_integer),	      
	      cmocka_unit_test(utest_binson_util_unpack_double),	      	      
	      };
  return cmocka_run_group_tests(tests, NULL, NULL);
}