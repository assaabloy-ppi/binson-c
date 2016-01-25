/*
 *	Unit tests for 'binson_token_buf' module
 */
#include <string.h>
#include <math.h>

#include "btest.h"

#include "binson/binson_token_buf.h"

static uint8_t buf[512];

/************************************************************/
static int setup(void **state) {

    binson_res             res;
    binson_io              *io;
    binson_token_buf       *tb;

    res = binson_io_new( &io );
    res = binson_io_attach_bytebuf( io, buf, sizeof(buf));    
    res = binson_token_buf_new( &tb );
    res = binson_token_buf_init( tb, NULL, 0, io );  /* dynamic token buffer with auto size change */
    
    *state = tb;
    
    UNUSED(res);
  
    return 0;
}

/************************************************************/
static int teardown(void **state) {

     binson_res		res;  
     binson_token_buf	*tb = *state;
     
     res = binson_io_free( binson_token_buf_get_io( tb ) );
     res = binson_token_buf_free( tb );

     UNUSED(res);     
     return 0;
}

#define UTEST_TB_START( sample ) \
  binson_io_seek( binson_token_buf_get_io( tb ), 0 ); \
  binson_token_buf_reset( tb ); \
  memcpy( buf, sample, sizeof(sample) );


/************************************************************/
static void utest_binson_token_buf_object(void **state) {
  binson_token_buf	*tb = *state;
  binson_res		res = BINSON_RES_OK;
  uint8_t		cnt = 0;
  uint8_t		sig = 0;
  bool 			cl = 0;
  binson_node_type      nt = BINSON_TYPE_UNKNOWN;

    

  UTEST_TB_START("\x40\x41"); 
  cnt = 2;
  res = binson_token_buf_token_fill( tb, &cnt );    assert_true(res == BINSON_RES_OK );
  assert_int_equal(cnt, 1);
  res = binson_token_buf_get_sig( tb, 0, &sig );    assert_true(res == BINSON_RES_OK );
  assert_int_equal(sig, 0x40);  
  res = binson_token_buf_token_fill( tb, &cnt );    assert_true(res == BINSON_RES_OK );
  assert_int_equal(cnt, 1);
  res = binson_token_buf_get_sig( tb, 1, &sig );    assert_true(res == BINSON_RES_OK );
  assert_int_equal(sig, 0x41);
  
  res = binson_token_buf_get_node_type( tb, 0, &nt, &cl );    assert_true(res == BINSON_RES_OK );
  assert_int_equal(nt, BINSON_TYPE_OBJECT);
  assert_true(!cl);
  res = binson_token_buf_get_node_type( tb, 1, &nt, &cl );    assert_true(res == BINSON_RES_OK );
  assert_int_equal(nt, BINSON_TYPE_OBJECT);
  assert_true(cl);
  
  
  
  UTEST_TB_START("\x14\x01\x61\x40\x41"); 
  cnt = 2;
  res = binson_token_buf_token_fill( tb, &cnt );    assert_true(res == BINSON_RES_OK );
  assert_int_equal(cnt, 2);
  res = binson_token_buf_get_sig( tb, 0, &sig );    assert_true(res == BINSON_RES_OK );
  assert_int_equal(sig, 0x14);
  res = binson_token_buf_get_sig( tb, 1, &sig );    assert_true(res == BINSON_RES_OK );
  assert_int_equal(sig, 0x40);
  
  res = binson_token_buf_get_node_type( tb, 0, &nt, &cl );    assert_true(res == BINSON_RES_OK );
  assert_int_equal(nt, BINSON_TYPE_STRING);
  assert_true(!cl);
  res = binson_token_buf_token_fill( tb, &cnt );		assert_true(res == BINSON_RES_OK );
  res = binson_token_buf_get_node_type( tb, 1, &nt, &cl );	assert_true(res == BINSON_RES_OK );
  assert_int_equal(nt, BINSON_TYPE_OBJECT);
  assert_true(!cl);
  
  cnt = 1;  
  res = binson_token_buf_reset( tb );   			assert_true(res == BINSON_RES_OK );
  res = binson_token_buf_token_fill( tb, &cnt );		assert_true(res == BINSON_RES_OK );
  assert_int_equal(cnt, 1);  
  res = binson_token_buf_get_sig( tb, 0, &sig );   		assert_true(res == BINSON_RES_OK );
  assert_int_equal(sig, 0x41);
  res = binson_token_buf_get_node_type( tb, 0, &nt, &cl );	assert_true(res == BINSON_RES_OK );
  assert_int_equal(nt, BINSON_TYPE_OBJECT);
  assert_true(cl);
  
}

/************************************************************/
static void utest_binson_token_buf_array(void **state) {
  binson_token_buf	*tb = *state;
  binson_res		res = BINSON_RES_OK;
  uint8_t		cnt = 0;
  uint8_t		sig = 0;
  bool 			cl = 0;
  binson_node_type      nt = BINSON_TYPE_UNKNOWN;

    

  UTEST_TB_START("\x42\x43"); 
  cnt = 2;
  res = binson_token_buf_token_fill( tb, &cnt );    assert_true(res == BINSON_RES_OK );
  assert_int_equal(cnt, 1);
  res = binson_token_buf_get_sig( tb, 0, &sig );    assert_true(res == BINSON_RES_OK );
  assert_int_equal(sig, 0x42);  
  res = binson_token_buf_token_fill( tb, &cnt );    assert_true(res == BINSON_RES_OK );
  assert_int_equal(cnt, 1);
  res = binson_token_buf_get_sig( tb, 1, &sig );    assert_true(res == BINSON_RES_OK );
  assert_int_equal(sig, 0x43);
  
  res = binson_token_buf_get_node_type( tb, 0, &nt, &cl );    assert_true(res == BINSON_RES_OK );
  assert_int_equal(nt, BINSON_TYPE_ARRAY);
  assert_true(!cl);
  res = binson_token_buf_get_node_type( tb, 1, &nt, &cl );    assert_true(res == BINSON_RES_OK );
  assert_int_equal(nt, BINSON_TYPE_ARRAY);
  assert_true(cl);
  
  
  
  UTEST_TB_START("\x14\x01\x61\x42\x43"); 
  cnt = 2;
  res = binson_token_buf_token_fill( tb, &cnt );    assert_true(res == BINSON_RES_OK );
  assert_int_equal(cnt, 2);
  res = binson_token_buf_get_sig( tb, 0, &sig );    assert_true(res == BINSON_RES_OK );
  assert_int_equal(sig, 0x14);
  res = binson_token_buf_get_sig( tb, 1, &sig );    assert_true(res == BINSON_RES_OK );
  assert_int_equal(sig, 0x42);
  
  res = binson_token_buf_get_node_type( tb, 0, &nt, &cl );    assert_true(res == BINSON_RES_OK );
  assert_int_equal(nt, BINSON_TYPE_STRING);
  assert_true(!cl);
  res = binson_token_buf_token_fill( tb, &cnt );		assert_true(res == BINSON_RES_OK );
  res = binson_token_buf_get_node_type( tb, 1, &nt, &cl );	assert_true(res == BINSON_RES_OK );
  assert_int_equal(nt, BINSON_TYPE_ARRAY);
  assert_true(!cl);
  
  cnt = 1;  
  res = binson_token_buf_reset( tb );   			assert_true(res == BINSON_RES_OK );
  res = binson_token_buf_token_fill( tb, &cnt );		assert_true(res == BINSON_RES_OK );
  assert_int_equal(cnt, 1);  
  res = binson_token_buf_get_sig( tb, 0, &sig );   		assert_true(res == BINSON_RES_OK );
  assert_int_equal(sig, 0x43);
  res = binson_token_buf_get_node_type( tb, 0, &nt, &cl );	assert_true(res == BINSON_RES_OK );
  assert_int_equal(nt, BINSON_TYPE_ARRAY);
  assert_true(cl);
  
}

/************************************************************/
static void utest_binson_token_buf_boolean(void **state) {
  binson_token_buf	*tb = *state;
  binson_res		res = BINSON_RES_OK;
  uint8_t		cnt = 0;
  uint8_t		sig = 0;
  bool 			cl = 0;
  binson_node_type      nt = BINSON_TYPE_UNKNOWN;
  binson_raw_value 	rv;
    
  UTEST_TB_START("\x44\x45"); 
  cnt = 1;
  res = binson_token_buf_token_fill( tb, &cnt );    assert_true(res == BINSON_RES_OK );    
  res = binson_token_buf_get_sig( tb, 0, &sig );    assert_true(res == BINSON_RES_OK );
  assert_int_equal(sig, 0x44);  
  res = binson_token_buf_get_node_type( tb, 0, &nt, &cl );    assert_true(res == BINSON_RES_OK );
  assert_int_equal(nt, BINSON_TYPE_BOOLEAN);
  assert_true(!cl);
  res = binson_token_buf_get_token_payload( tb, 0, &rv  );    assert_true(res == BINSON_RES_OK );
  assert_true( rv.bool_val );
  
  res = binson_token_buf_reset( tb );   			assert_true(res == BINSON_RES_OK );  
  res = binson_token_buf_token_fill( tb, &cnt );    assert_true(res == BINSON_RES_OK );
  assert_int_equal(cnt, 1);
  res = binson_token_buf_get_sig( tb, 0, &sig );    assert_true(res == BINSON_RES_OK );
  assert_int_equal(sig, 0x45);
  res = binson_token_buf_get_node_type( tb, 0, &nt, &cl );    assert_true(res == BINSON_RES_OK );
  assert_int_equal(nt, BINSON_TYPE_BOOLEAN);
  assert_true(!cl);
  res = binson_token_buf_get_token_payload( tb, 0, &rv  );    assert_true(res == BINSON_RES_OK );
  assert_true( !rv.bool_val );     
}

/************************************************************/
static void utest_binson_token_buf_integer(void **state) {
  binson_token_buf	*tb = *state;
  binson_res		res = BINSON_RES_OK;
  uint8_t		cnt = 0;
  bool 			cl = 0;
  binson_node_type      nt = BINSON_TYPE_UNKNOWN;
  binson_raw_value 	rv;

#define UTEST_TB_INT( i ) \
  cnt = 1; \
  res = binson_token_buf_token_fill( tb, &cnt );    	      assert_true(res == BINSON_RES_OK );    \
  res = binson_token_buf_get_node_type( tb, 0, &nt, &cl );    assert_true(res == BINSON_RES_OK );	\
  res = binson_token_buf_get_token_payload( tb, 0, &rv  );    assert_true(res == BINSON_RES_OK );	\
  assert_int_equal(nt, BINSON_TYPE_INTEGER);	\
  assert_true(!cl);	\
  assert_int_equal( rv.int_val, i);
    
  UTEST_TB_START("\x10\x00"); 
  UTEST_TB_INT( 0 );
  
  UTEST_TB_START("\x10\x01"); 
  UTEST_TB_INT( 1 );

  UTEST_TB_START("\x10\xff"); 
  UTEST_TB_INT( -1 );

  UTEST_TB_START("\x11\x7e\xff"); 
  UTEST_TB_INT( -130 );

  UTEST_TB_START("\x11\x82\x00"); 
  UTEST_TB_INT( 130 );

  UTEST_TB_START("\x12\xff\xff\xff\x7f"); 
  UTEST_TB_INT( INT32_MAX );

  UTEST_TB_START("\x12\x00\x00\x00\x80"); 
  UTEST_TB_INT( INT32_MIN );

  UTEST_TB_START("\x13\xff\xff\xff\xff\xff\xff\xff\x7f"); 
  UTEST_TB_INT( INT64_MAX );

  UTEST_TB_START("\x13\x00\x00\x00\x00\x00\x00\x00\x80"); 
  UTEST_TB_INT( INT64_MIN );

  UTEST_TB_START("\x13\xff\xff\xff\xff\xff\xff\xff\x3f"); 
  UTEST_TB_INT( INT64_MAX / 2 );

  UTEST_TB_START("\x13\x01\x00\x00\x00\x00\x00\x00\xc0"); 
  UTEST_TB_INT(  -(INT64_MAX / 2) );
}

/************************************************************/
static void utest_binson_token_buf_double(void **state) {
  binson_token_buf	*tb = *state;
  binson_res		res = BINSON_RES_OK;
  uint8_t		cnt = 0;
  bool 			cl = 0;
  binson_node_type      nt = BINSON_TYPE_UNKNOWN;
  binson_raw_value 	rv;

#define UTEST_TB_DOUBLE( d ) \
  cnt = 1; \
  res = binson_token_buf_token_fill( tb, &cnt );    	      assert_true(res == BINSON_RES_OK );    \
  res = binson_token_buf_get_node_type( tb, 0, &nt, &cl );    assert_true(res == BINSON_RES_OK );	\
  res = binson_token_buf_get_token_payload( tb, 0, &rv  );    assert_true(res == BINSON_RES_OK );	\
  assert_int_equal(nt, BINSON_TYPE_DOUBLE);	\
  assert_true(!cl);	\
  if (isnan(d))	\
    assert_true( isnan(rv.double_val)); 	\
  else 	\
    assert_true( rv.double_val == d);    	\
  
  UTEST_TB_START("\x46\x00\x00\x00\x00\x00\x00\x00\x00"); 
  UTEST_TB_DOUBLE( 0 );
  
  UTEST_TB_START("\x46\x00\x00\x00\x00\x00\x00\xf0\xbf"); 
  UTEST_TB_DOUBLE( -1 );
  
  UTEST_TB_START("\x46\x6f\x12\x83\xc0\xca\x21\x09\x40"); 
  UTEST_TB_DOUBLE( 3.1415 );
  
  UTEST_TB_START("\x46\x80\xfe\x91\x5a\x98\xb0\x46\x49"); 
  UTEST_TB_DOUBLE( 1.012e+45 );
  
  UTEST_TB_START("\x46\x00\x00\x00\x00\x00\x00\xf8\x7f"); 
  UTEST_TB_DOUBLE( NAN );
  
  UTEST_TB_START("\x46\x00\x00\x00\x00\x00\x00\xf0\x7f"); 
  UTEST_TB_DOUBLE( INFINITY );
  
  UTEST_TB_START("\x46\x00\x00\x00\x00\x00\x00\xf0\xff"); 
  UTEST_TB_DOUBLE( -INFINITY );
}

/************************************************************/
static void utest_binson_token_buf_str(void **state) {
  binson_token_buf	*tb = *state;
  binson_res		res = BINSON_RES_OK;
  uint8_t		cnt = 0;
  bool 			cl = 0;
  binson_node_type      nt = BINSON_TYPE_UNKNOWN;
  binson_raw_value 	rv;
  
  uint8_t 		bbb[64];

#define UTEST_TB_STR( s ) \
  cnt = 1; \
  res = binson_token_buf_token_fill( tb, &cnt );    	      assert_true(res == BINSON_RES_OK );    \
  res = binson_token_buf_get_node_type( tb, 0, &nt, &cl );    assert_true(res == BINSON_RES_OK );	\
  res = binson_token_buf_get_token_payload( tb, 0, &rv  );    assert_true(res == BINSON_RES_OK );	\
  assert_int_equal(nt, BINSON_TYPE_STRING);	\
  assert_true(!cl); \
  memcpy(bbb, rv.bbuf_val.bptr, rv.bbuf_val.bsize ); \
  bbb[rv.bbuf_val.bsize] = '\0'; \
  assert_string_equal( bbb, s ); 
  
  UTEST_TB_START("\x14\x00"); 
  UTEST_TB_STR( "" );

  UTEST_TB_START("\x14\x06\x73\x74\x72\x69\x6e\x67"); 
  UTEST_TB_STR( "string" );

  UTEST_TB_START("\x14\x08\x67\x72\xc3\xb6\xc3\x9f\x65\x72"); 
  UTEST_TB_STR( "größer" );

  UTEST_TB_START("\x14\x09\xe6\xb7\x98\xe5\xae\x9d\xe7\xbd\x91"); 
  UTEST_TB_STR( "\u6DD8\u5B9D\u7F51" );
}

/************************************************************/
static void utest_binson_token_buf_bytes(void **state) {
  binson_token_buf	*tb = *state;
  binson_res		res = BINSON_RES_OK;
  uint8_t		cnt = 0;
  bool 			cl = 0;
  binson_node_type      nt = BINSON_TYPE_UNKNOWN;
  binson_raw_value 	rv;

#define UTEST_TB_BYTES( b ) \
  cnt = 1; \
  res = binson_token_buf_token_fill( tb, &cnt );    	      assert_true(res == BINSON_RES_OK );    \
  res = binson_token_buf_get_node_type( tb, 0, &nt, &cl );    assert_true(res == BINSON_RES_OK );	\
  res = binson_token_buf_get_token_payload( tb, 0, &rv  );    assert_true(res == BINSON_RES_OK );	\
  assert_int_equal(nt, BINSON_TYPE_BYTES);	\
  assert_true(!cl); \
  assert_memory_equal( rv.bbuf_val.bptr, b, sizeof(b) );
    
  UTEST_TB_START("\x18\x00"); 
  UTEST_TB_BYTES( "" );
  
  UTEST_TB_START("\x18\x05\x61\x62\x63\x64\x00"); 
  UTEST_TB_BYTES( "abcd\x00" );

  UTEST_TB_START("\x18\x05\x61\x62\x63\x64\x00"); 
  UTEST_TB_BYTES( "abcd\x00" );

  UTEST_TB_START("\x18\x04\x00\x00\x00\x00"); 
  UTEST_TB_BYTES( "\x00\x00\x00\x00" );

  UTEST_TB_START("\x18\x03\x00\x00\xff"); 
  UTEST_TB_BYTES( "\x00\x00\xff" );
}

/************************************************************/
static void utest_binson_token_buf_invalid_input(void **state) {
  binson_token_buf	*tb = *state;
  binson_res		res = BINSON_RES_OK;
  uint8_t		cnt = 0;

    
  UTEST_TB_START(""); /* empty input */
  cnt = 1;
  res = binson_token_buf_token_fill( tb, &cnt );    assert_true(res == BINSON_RES_ERROR_PARSE_INVALID_INPUT );

  UTEST_TB_START("\x00"); 
  cnt = 1;
  res = binson_token_buf_token_fill( tb, &cnt );    assert_true(res == BINSON_RES_ERROR_PARSE_INVALID_INPUT );

  UTEST_TB_START("\x20\x20"); 
  cnt = 1;
  res = binson_token_buf_token_fill( tb, &cnt );    assert_true(res == BINSON_RES_ERROR_PARSE_INVALID_INPUT );

  UTEST_TB_START("\x40\x20\x41"); 
  cnt = 2;
  res = binson_token_buf_token_fill( tb, &cnt );    assert_true(res == BINSON_RES_ERROR_PARSE_INVALID_INPUT );
    
}

/************************************************************/
int utest_run_tests(void) {
  const struct CMUnitTest tests[] = {
            cmocka_unit_test_setup_teardown(utest_binson_token_buf_object, setup, teardown),
            cmocka_unit_test_setup_teardown(utest_binson_token_buf_array, setup, teardown),            
            cmocka_unit_test_setup_teardown(utest_binson_token_buf_boolean, setup, teardown),            
            cmocka_unit_test_setup_teardown(utest_binson_token_buf_integer, setup, teardown),            
            cmocka_unit_test_setup_teardown(utest_binson_token_buf_double, setup, teardown),         
            cmocka_unit_test_setup_teardown(utest_binson_token_buf_str, setup, teardown),            
            cmocka_unit_test_setup_teardown(utest_binson_token_buf_bytes, setup, teardown),                        
            
            cmocka_unit_test_setup_teardown(utest_binson_token_buf_invalid_input, setup, teardown),                
  };
  
  return cmocka_run_group_tests(tests, NULL, NULL);
}