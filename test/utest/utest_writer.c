/*
 *	Unit tests for 'binson_writer' module
 */
#include <string.h>
#include <math.h>

#include "btest.h"

#include "binson/binson_writer.h"

static uint8_t buf[512];

/************************************************************/
static int setup(void **state) {

    binson_res             res;
    binson_io              *io;
    binson_writer          *writer;

    res = binson_io_new( &io );
    res = binson_io_attach_bytebuf( io, buf, sizeof(buf));
    res = binson_writer_new( &writer );
    res = binson_writer_init( writer, io, BINSON_WRITER_FORMAT_RAW);

    *state = writer;
    
    UNUSED(res);
  
    return 0;
}

/************************************************************/
static int teardown(void **state) {

     binson_res		res;  
     binson_writer	*writer = *state;
     
     res = binson_io_free( binson_writer_get_io( writer ) );
     res = binson_writer_free( writer );

     UNUSED(res);     
     return 0;
}

#define UTEST_WRITER_START() \
  binson_io_seek( binson_writer_get_io( writer ), 0 );	\
  binson_io_reset_counters( binson_writer_get_io( writer ) );
  
#define UTEST_WRITER_END( sample ) \
  binson_io_get_write_counter( binson_writer_get_io( writer ), &cnt ); \
    \
  assert_int_equal(cnt, sizeof(sample)-1); \
  assert_memory_equal(buf, sample, cnt); \


/************************************************************/
static void utest_binson_writer_write_object(void **state) {
   
  binson_writer		*writer = *state;
  binson_res		res = BINSON_RES_OK;
  binson_raw_size	cnt=0;

  UNUSED(res);
  
  UTEST_WRITER_START();
  binson_writer_write_object_begin( writer, NULL );
  binson_writer_write_object_end( writer );
  UTEST_WRITER_END("\x40\x41");

  UTEST_WRITER_START();
  binson_writer_write_object_begin( writer, "a" );
  binson_writer_write_object_begin( writer, "größer" );
  binson_writer_write_object_end( writer );  
  binson_writer_write_object_end( writer );
  UTEST_WRITER_END("\x14\x01\x61\x40\x14\x08\x67\x72\xC3\xB6\xC3\x9F\x65\x72\x40\x41\x41");  
}

/************************************************************/
static void utest_binson_writer_write_array(void **state) {
  (void) state;

  binson_writer		*writer = *state;
  binson_res		res = BINSON_RES_OK;
  binson_raw_size	cnt=0;

  UNUSED(res);
  
  UTEST_WRITER_START();
  binson_writer_write_array_begin( writer, NULL );
  binson_writer_write_array_end( writer );
  UTEST_WRITER_END("\x42\x43");    

  UTEST_WRITER_START();
  binson_writer_write_array_begin( writer, "a" );
  binson_writer_write_array_begin( writer, "größer" );
  binson_writer_write_array_end( writer );  
  binson_writer_write_array_end( writer );
  /* binson_writer is low level layer for serialization of tokens and it doesn't monitor tree structure */    
  /* thus writing item's key for ARRAY parrent is ok on this level */      
  UTEST_WRITER_END("\x14\x01\x61\x42\x14\x08\x67\x72\xC3\xB6\xC3\x9F\x65\x72\x42\x43\x43");   
}

/************************************************************/
static void utest_binson_writer_write_boolean(void **state) {
  (void) state;
  binson_writer		*writer = *state;
  binson_res		res = BINSON_RES_OK;
  binson_raw_size	cnt=0;

  UNUSED(res);
  
  UTEST_WRITER_START();
  binson_writer_write_boolean( writer, NULL, true );
  UTEST_WRITER_END("\x44");   

  UTEST_WRITER_START();
  binson_writer_write_boolean( writer, NULL, 5 );
  UTEST_WRITER_END("\x44");   
  
  UTEST_WRITER_START();
  binson_writer_write_boolean( writer, NULL, -1 );  /* true is not zero */
  UTEST_WRITER_END("\x44");   
  
  UTEST_WRITER_START();
  binson_writer_write_boolean( writer, NULL, 1 );
  UTEST_WRITER_END("\x44");   
  
  UTEST_WRITER_START();
  binson_writer_write_boolean( writer, NULL, 0 );
  UTEST_WRITER_END("\x45");   
  
  UTEST_WRITER_START();
  binson_writer_write_boolean( writer, "größer", false );
  UTEST_WRITER_END("\x14\x08\x67\x72\xC3\xB6\xC3\x9F\x65\x72\x45");         
}

/************************************************************/
static void utest_binson_writer_write_integer(void **state) {
  (void) state;
  binson_writer		*writer = *state;
  binson_res		res = BINSON_RES_OK;
  binson_raw_size	cnt=0;

  UNUSED(res);

  UTEST_WRITER_START(); 
  binson_writer_write_integer( writer, NULL, 0 );
  UTEST_WRITER_END("\x10\x00");       
  
  UTEST_WRITER_START(); 
  binson_writer_write_integer( writer, NULL, 1 );
  UTEST_WRITER_END("\x10\x01");     
  
  UTEST_WRITER_START(); 
  binson_writer_write_integer( writer, NULL, -1 );
  UTEST_WRITER_END("\x10\xff");     

  UTEST_WRITER_START(); 
  binson_writer_write_integer( writer, NULL, -130 );  
  UTEST_WRITER_END("\x11\x7e\xff");     

  UTEST_WRITER_START(); 
  binson_writer_write_integer( writer, NULL, 130 );    
  UTEST_WRITER_END("\x11\x82\x00");     
  
  UTEST_WRITER_START(); 
  binson_writer_write_integer( writer, NULL, INT32_MAX );
  UTEST_WRITER_END("\x12\xff\xff\xff\x7f");     

  UTEST_WRITER_START(); 
  binson_writer_write_integer( writer, NULL, INT32_MIN );
  UTEST_WRITER_END("\x12\x00\x00\x00\x80");     

  UTEST_WRITER_START(); 
  binson_writer_write_integer( writer, NULL, INT64_MAX );
  UTEST_WRITER_END("\x13\xff\xff\xff\xff\xff\xff\xff\x7f");     

  UTEST_WRITER_START(); 
  binson_writer_write_integer( writer, NULL, INT64_MIN );
  UTEST_WRITER_END("\x13\x00\x00\x00\x00\x00\x00\x00\x80");     

  UTEST_WRITER_START(); 
  binson_writer_write_integer( writer, NULL, INT64_MAX / 2 );
  UTEST_WRITER_END("\x13\xff\xff\xff\xff\xff\xff\xff\x3f");     

  UTEST_WRITER_START(); 
  binson_writer_write_integer( writer, NULL, -(INT64_MAX / 2) );    
  UTEST_WRITER_END("\x13\x01\x00\x00\x00\x00\x00\x00\xc0");        
}

/************************************************************/
static void utest_binson_writer_write_double(void **state) {
  (void) state;
  binson_writer		*writer = *state;
  binson_res		res = BINSON_RES_OK;
  binson_raw_size	cnt=0;
  
  UNUSED(res);

  UTEST_WRITER_START();
  binson_writer_write_double( writer, NULL, 0 );    
  UTEST_WRITER_END("\x46\x00\x00\x00\x00\x00\x00\x00\x00");   
  
  UTEST_WRITER_START();
  binson_writer_write_double( writer, NULL, -1 );  
  UTEST_WRITER_END("\x46\x00\x00\x00\x00\x00\x00\xf0\xbf");   

  UTEST_WRITER_START();
  binson_writer_write_double( writer, NULL, 3.1415 );
  UTEST_WRITER_END("\x46\x6f\x12\x83\xc0\xca\x21\x09\x40");   

  UTEST_WRITER_START();
  binson_writer_write_double( writer, NULL, 1.012e+45 );
  UTEST_WRITER_END("\x46\x80\xfe\x91\x5a\x98\xb0\x46\x49");   

  UTEST_WRITER_START();
  binson_writer_write_double( writer, NULL, NAN );
  UTEST_WRITER_END("\x46\x00\x00\x00\x00\x00\x00\xf8\x7f");   
  
  UTEST_WRITER_START();
  binson_writer_write_double( writer, NULL, INFINITY );  
  UTEST_WRITER_END("\x46\x00\x00\x00\x00\x00\x00\xf0\x7f");   
  
  UTEST_WRITER_START();
  binson_writer_write_double( writer, NULL, -INFINITY );  
  UTEST_WRITER_END("\x46\x00\x00\x00\x00\x00\x00\xf0\xff");     
}

/************************************************************/
static void utest_binson_writer_write_str(void **state) {
  (void) state;
  binson_writer		*writer = *state;
  binson_res		res = BINSON_RES_OK;
  binson_raw_size	cnt=0;

  UNUSED(res);

  UTEST_WRITER_START();
  binson_writer_write_str( writer, NULL, NULL );  /* NULL */
  UTEST_WRITER_END("");         
  
  UTEST_WRITER_START();
  binson_writer_write_str( writer, NULL, "" );  /* empty */
  UTEST_WRITER_END("\x14\x00");       

  UTEST_WRITER_START();
  binson_writer_write_str( writer, "a", "" );  /* empty */
  UTEST_WRITER_END("\x14\x01\x61\x14\x00");       

  UTEST_WRITER_START();
  binson_writer_write_str( writer, "", "" );  /* empty */
  UTEST_WRITER_END("\x14\x00\x14\x00");       
  
  
  UTEST_WRITER_START();
  binson_writer_write_str( writer, NULL, "string" );
  UTEST_WRITER_END("\x14\x06\x73\x74\x72\x69\x6e\x67");       

  UTEST_WRITER_START();
  binson_writer_write_str( writer, "a", "b" );
  UTEST_WRITER_END("\x14\x01\x61\x14\x01\x62");         

  UTEST_WRITER_START();
  binson_writer_write_str( writer, "ö", "ß" );
  UTEST_WRITER_END("\x14\x02\xc3\xb6\x14\x02\xc3\x9f");         

  UTEST_WRITER_START(); 
  binson_writer_write_str( writer, "\u6DD8\u5B9D\u7F51", "\u6DD8\u5B9D\u7F51" );  /* utf-8 encoded 淘宝网 */
  UTEST_WRITER_END("\x14\x09\xe6\xb7\x98\xe5\xae\x9d\xe7\xbd\x91\x14\x09\xe6\xb7\x98\xe5\xae\x9d\xe7\xbd\x91");           
}

/************************************************************/
static void utest_binson_writer_write_bytes(void **state) {
  (void) state;
  binson_writer		*writer = *state;
  binson_res		res = BINSON_RES_OK;
  binson_raw_size	cnt=0;

  uint8_t 		testbuf[128];
  
  UNUSED(res);

  UTEST_WRITER_START();
  binson_writer_write_bytes( writer, NULL, NULL, 0 ); 
  UTEST_WRITER_END(""); 

  UTEST_WRITER_START();
  binson_writer_write_bytes( writer, NULL, NULL, 100 ); 
  UTEST_WRITER_END("");   

  UTEST_WRITER_START();
  binson_writer_write_bytes( writer, NULL, (uint8_t*)writer, 0 ); 
  UTEST_WRITER_END("\x18\x00");     

  strcpy((char*)testbuf, "abcd");

  UTEST_WRITER_START();
  binson_writer_write_bytes( writer, NULL, testbuf, 2 ); 
  UTEST_WRITER_END("\x18\x02\x61\x62");     
  
  UTEST_WRITER_START();
  binson_writer_write_bytes( writer, NULL, testbuf, 5 ); 
  UTEST_WRITER_END("\x18\x05\x61\x62\x63\x64\x00");     
  
  memset(testbuf, 0, 4);
  UTEST_WRITER_START();
  binson_writer_write_bytes( writer, NULL, testbuf, 4 ); 
  UTEST_WRITER_END("\x18\x04\x00\x00\x00\x00");       
}

/************************************************************/
int utest_run_tests(void) {
  const struct CMUnitTest tests[] = {
            cmocka_unit_test_setup_teardown(utest_binson_writer_write_object, setup, teardown),
            cmocka_unit_test_setup_teardown(utest_binson_writer_write_array, setup, teardown),
            cmocka_unit_test_setup_teardown(utest_binson_writer_write_boolean, setup, teardown),
            cmocka_unit_test_setup_teardown(utest_binson_writer_write_integer, setup, teardown),
            cmocka_unit_test_setup_teardown(utest_binson_writer_write_double, setup, teardown),
            cmocka_unit_test_setup_teardown(utest_binson_writer_write_str, setup, teardown),
            cmocka_unit_test_setup_teardown(utest_binson_writer_write_bytes, setup, teardown),
  };
  
  return cmocka_run_group_tests(tests, NULL, NULL);
}
