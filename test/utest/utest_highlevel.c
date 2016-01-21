/*
 *	High level unit tests for binson-c library
 */
#include <string.h>
#include <math.h>

#include "btest.h"

#include "binson/binson.h"

/* composite context used to pass ref via single pointer */
typedef struct binson_composite
{
    binson          *obj;
    binson_writer   *writer;
    binson_parser   *parser;
    binson_io       *io_in;
    binson_io       *io_out;
    binson_io       *io_err;
       
} binson_composite;

static uint8_t sbuf[1024];
static uint8_t dbuf[1024];

const uint8_t s0[]  = "\x40\x41"; // {}
const uint8_t s1[]  = "\x40\x14\x00\x40\x41\x41"; // {"":{}}
const uint8_t s2[] = "\x40\x14\x00\x40\x14\x00\x40\x14\x00\x40\x41\x41\x41\x41"; // {"":{"":{"":{}}}}
const uint8_t s3[]  = "\x40\x14\x08\x67\x72\xc3\xb6\xc3\x9f\x65\x72\x40\x41\x41"; // {"größer":{}}
const uint8_t s4[]  = "\x40\x14\x08\x67\x72\xc3\xb6\xc3\x9f\x65\x72\x40\x14\x09\xe6\xb7\x98\xe5\xae\x9d\xe7\xbd\x91\x40\x41\x41\x41"; // {"größer":{"淘宝网":{}}}
const uint8_t s5[]  = "\x40\x14\x03\x61\x62\x63\x44\x14\x03\x63\x62\x61\x45\x41"; //{"abc":true, "cba":false}
const uint8_t s6[]  = "\x40\x14\x01\x61\x40\x41\x14\x01\x62\x10\x6f\x41"; // {"a":{}, "b":111}
const uint8_t s7[]  = "\x40\x14\x03\x61\x62\x63\x40\x41\x14\x03\x63\x62\x61\x40\x41\x41"; // {"abc":{}, "cba":{}}
const uint8_t s8[]  = "\x40\x14\x01\x62\x40\x14\x01\x61\x40\x41\x41\x14\x01\x64\x40\x14\x01\x63\x40\x41\x41\x41"; // {"b":{"a":{}}, "d":{"c":{}}}
const uint8_t s9[]  = "\x40\x14\x08\x67\x72\xc3\xb6\xc3\x9f\x65\x72\x40\x41\x14\x09\xe6\xb7\x98\xe5\xae\x9d\xe7\xbd\x91\x40\x41\x41"; // {"größer":{}, "淘宝网":{}}
const uint8_t s10[]  = "\x40\x14\x01\x61\x40\x41\x14\x03\x62\x63\x61\x40\x41\x14\x03\x63\x62\x61\x40\x41\x41"; // {"a":{}, "bca":{}, "cba":{}}

const uint8_t sa1[]  = "\x40\x14\x00\x40\x41\x41"; // {"":{}}
const uint8_t sa2[] = "\x40\x14\x00\x42\x42\x42\x43\x43\x43\x41"; // {"":[[[]]]}
const uint8_t sa3[]  = "\x40\x14\x01\x61\x42\x42\x43\x42\x43\x43\x41"; // {"a":[[],[]]}
const uint8_t sa4[]  = "\x40\x14\x01\x61\x42\x42\x43\x42\x43\x40\x41\x43\x41"; // {"a":[[],[],{}]}
const uint8_t sa5[]  = "\x40\x14\x01\x61\x42\x42\x40\x41\x43\x42\x40\x41\x43\x43\x41"; // {"a":[[{}],[{}]]}

// {"a":[true,13,-2.34,"zxc",{"d":false, "e":"0x030405", "q":"qwe"},9223372036854775807]}
const uint8_t sb1[]  = "\x40\x14\x01\x61\x42\x44\x10\x0d\x46\xb8\x1e\x85\xeb\x51\xb8\x02\xc0\x14\x03\x7a\x78\x63\x40\x14\x01\x64\x45\x14\x01\x65\x18\x03\x03\x04\x05\x14\x01\x71\x14\x03\x71\x77\x65\x41\x13\xff\xff\xff\xff\xff\xff\xff\x7f\x43\x41";

/************************************************************/
static int global_setup(void **state) {
    UNUSED(state);    
    binson_composite *bc = *state = (binson_composite *)malloc( sizeof(binson_composite) );      
    binson_res       res;

    UNUSED(state);    
    UNUSED(res);        
    
    res = binson_io_new( &bc->io_in );
    res = binson_io_new( &bc->io_out );
    res = binson_io_new( &bc->io_err );

    res = binson_io_attach_stream( bc->io_err, stdout );
    res = binson_io_attach_bytebuf( bc->io_in, sbuf, sizeof(sbuf) );
    res = binson_io_attach_bytebuf( bc->io_out, dbuf, sizeof(dbuf) );
    
    res = binson_parser_new( &bc->parser );
    res = binson_parser_init( bc->parser, bc->io_in, BINSON_PARSER_MODE_DOM );

    res = binson_writer_new( &bc->writer );
    res = binson_writer_init( bc->writer, bc->io_out, BINSON_WRITER_FORMAT_RAW );

    res = binson_new( &bc->obj );
    res = binson_init( bc->obj, bc->io_err );
    
    return 0;  
}

/************************************************************/
static int global_teardown(void **state) {
    UNUSED(state);
    binson_composite *bc = *state;
    
    binson_writer_free( bc->writer );
    binson_parser_free( bc->parser );
    binson_io_free( bc->io_in );
    binson_io_free( bc->io_out );
    binson_io_free( bc->io_err );        
    binson_free( bc->obj );
    
    free( bc );
    
    return 0;      
}

/************************************************************/
static int setup(void **state) {
    UNUSED(state);
    binson_composite *bc = *state;

    binson_io_seek( binson_parser_get_io( bc->parser ), 0 );    
    binson_io_seek( binson_writer_get_io( bc->writer ), 0 );   
    
    return 0;
}

/************************************************************/
static int teardown(void **state) {
    UNUSED(state);
    binson_composite *bc = *state;
 
    UNUSED(state);
    UNUSED(bc);     
    
    return 0;
}

/************************************************************/
static void utest_highlevel_tree_build(void **state) {
    UNUSED(state);
    binson_composite *bc = *state;
    binson_res       res;
    binson_raw_size  rs;
    binson_node*     np[16];      
    
    UNUSED(res);
  
#define UTEST_HL_TREE_BUILD( sample ) \
    binson_io_seek( binson_writer_get_io( bc->writer ), 0 );    \
    res = binson_serialize( bc->obj, bc->writer, &rs );   assert_int_equal(res, BINSON_RES_OK ); \
    assert_int_equal( rs, sizeof(sample)-1 );  \
    assert_memory_equal( sample, dbuf, rs );     
    
    res = binson_reset( bc->obj );
    res = binson_node_add_object_empty( bc->obj, binson_get_root(bc->obj), "",  NULL );  assert_int_equal(res, BINSON_RES_OK ); 
    UTEST_HL_TREE_BUILD( s1 );    
    
    res = binson_reset( bc->obj );
    res = binson_node_add_object_empty( bc->obj, binson_get_root(bc->obj), "größer",  NULL );  assert_int_equal(res, BINSON_RES_OK ); 
    UTEST_HL_TREE_BUILD( s2 );    

    res = binson_reset( bc->obj );
    res = binson_node_add_object_empty( bc->obj, binson_get_root(bc->obj), "größer",  &np[0] );  assert_int_equal(res, BINSON_RES_OK ); 
    res = binson_node_add_object_empty( bc->obj, np[0], "淘宝网",  NULL );  assert_int_equal(res, BINSON_RES_OK );     
    UTEST_HL_TREE_BUILD( s3 );    
    
    // {"abc":true, "cba":false}
    res = binson_reset( bc->obj );
    res = binson_node_add_boolean( bc->obj, binson_get_root(bc->obj), "cba",  NULL, false);  assert_int_equal(res, BINSON_RES_OK ); 
    res = binson_node_add_boolean( bc->obj, binson_get_root(bc->obj), "abc",  NULL, true );  assert_int_equal(res, BINSON_RES_OK );     
    UTEST_HL_TREE_BUILD( s4 );   
    
    // {"a":{}, "b":111}
    res = binson_reset( bc->obj );
    res = binson_node_add_integer( bc->obj, binson_get_root(bc->obj), "b",  NULL, 111);  assert_int_equal(res, BINSON_RES_OK ); 
    res = binson_node_add_object_empty( bc->obj, binson_get_root(bc->obj), "a",  NULL );  assert_int_equal(res, BINSON_RES_OK );     
    UTEST_HL_TREE_BUILD( s5 ); 
    
    
    
    // {"abc":{}, "cba":{}}
    res = binson_reset( bc->obj );
    res = binson_node_add_object_empty( bc->obj, binson_get_root(bc->obj), "cba",  NULL);  assert_int_equal(res, BINSON_RES_OK ); 
    res = binson_node_add_object_empty( bc->obj, binson_get_root(bc->obj), "abc",  NULL );  assert_int_equal(res, BINSON_RES_OK );     
    UTEST_HL_TREE_BUILD( s6 );        
    
    // {"b":{"a":{}}, "d":{"c":{}}}
    /*res = binson_reset( bc->obj );
    res = binson_node_add_object_empty( bc->obj, binson_get_root(bc->obj), "d",  &np[0] );  assert_int_equal(res, BINSON_RES_OK ); 
    res = binson_node_add_object_empty( bc->obj, np[0], "c",  NULL );  assert_int_equal(res, BINSON_RES_OK );     
    res = binson_node_add_object_empty( bc->obj, binson_get_root(bc->obj), "b",  &np[1] );  assert_int_equal(res, BINSON_RES_OK ); 
    res = binson_node_add_object_empty( bc->obj, np[1], "a",  NULL );  assert_int_equal(res, BINSON_RES_OK );     
    UTEST_HL_TREE_BUILD( s7 );            
    */
    // {"größer":{}, "淘宝网":{}}
    res = binson_reset( bc->obj );
    res = binson_node_add_object_empty( bc->obj, binson_get_root(bc->obj), "größer",  NULL);  assert_int_equal(res, BINSON_RES_OK ); 
    res = binson_node_add_object_empty( bc->obj, binson_get_root(bc->obj), "淘宝网",  NULL );  assert_int_equal(res, BINSON_RES_OK );     
    UTEST_HL_TREE_BUILD( s8 );    
    
    
/*    res = binson_node_add_object_empty( bc->obj, binson_get_root(bc->obj), "cba",  NULL );  assert_int_equal(res, BINSON_RES_OK ); 
    res = binson_node_add_object_empty( bc->obj, binson_get_root(bc->obj), "bca",  NULL );  assert_int_equal(res, BINSON_RES_OK ); 
    res = binson_node_add_object_empty( bc->obj, binson_get_root(bc->obj), "a",  NULL );  assert_int_equal(res, BINSON_RES_OK );         
    UTEST_HL_TREE_BUILD( s14 );
    */
}

/************************************************************/
static void utest_highlevel_recycle(void **state) {
    UNUSED(state);
    binson_composite *bc = *state;
    binson_res       res;
    binson_raw_size  rs;
    
    UNUSED(res);
  
#define UTEST_HL_RECYCLE( sample ) \
    binson_io_seek( binson_parser_get_io( bc->parser ), 0 );   \
    binson_io_seek( binson_writer_get_io( bc->writer ), 0 );    \
    memcpy(sbuf, sample, sizeof(sample)-1); \
    res = binson_deserialize( bc->obj, bc->parser, NULL, NULL, false );    assert_int_equal(res, BINSON_RES_OK ); \
    res = binson_serialize( bc->obj, bc->writer, &rs );  		   assert_int_equal(res, BINSON_RES_OK ); \
    assert_int_equal( rs, sizeof(sample)-1 ); \
    assert_memory_equal( sbuf, dbuf, rs ); 
  
    
    
    UTEST_HL_RECYCLE( s0 );        
    UTEST_HL_RECYCLE( s1 );    
    UTEST_HL_RECYCLE( s2 );    
    UTEST_HL_RECYCLE( s3 );
    UTEST_HL_RECYCLE( s4 );    
    UTEST_HL_RECYCLE( s5 );    
    UTEST_HL_RECYCLE( s6 );    
    UTEST_HL_RECYCLE( s7 );    
    UTEST_HL_RECYCLE( s8 );    
    UTEST_HL_RECYCLE( s9 );    
    UTEST_HL_RECYCLE( s10 );  
    
    UTEST_HL_RECYCLE( sa1 );  
    UTEST_HL_RECYCLE( sa2 );  
    UTEST_HL_RECYCLE( sa3 );  
    UTEST_HL_RECYCLE( sa4 );  
    UTEST_HL_RECYCLE( sa5 );      

    UTEST_HL_RECYCLE( sb1 );      
}

/************************************************************/
int utest_run_tests(void) {
  const struct CMUnitTest tests[] = {
            //cmocka_unit_test_setup_teardown(utest_highlevel_tree_build, setup, teardown),    
            cmocka_unit_test_setup_teardown(utest_highlevel_recycle, setup, teardown),            
  };
  
  return cmocka_run_group_tests(tests, global_setup, global_teardown);
}
