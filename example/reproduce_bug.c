/**
 *  Template usefull for quick bugreport checks
 */

#include <stdio.h>
#include <string.h>

#include "binson/binson.h"

int bug1()
{
    // 2016-01-05, bug in lib or with Frans's understanding of it and C-programming
    // I get Segmentation fault in binson_deserialize().
    // Trying to build Binson object from byte buffer.

    binson *obj;
    binson_res res;
    binson_parser *parser;
    binson_io *in;
    binson_io *err;
    binson_node *node;
    int n;
    
    uint8_t buffer[] = {0x40, 0x41};    // empty Binson object
    int buffer_size = 2;

    printf("---- recreate_bug1() ----\n");

    res = binson_new(&obj);
    res = binson_io_new(&in);
    res = binson_io_new(&err);
    res = binson_parser_new(&parser);

    printf("bug1: binson structs allocated\n");

    res = binson_io_attach_bytebuf(in, buffer, buffer_size);
    res = binson_io_attach_stream(err, stderr);
    res = binson_parser_init(parser, in, BINSON_PARSER_MODE_DOM);
    res = binson_init(obj, err);

    printf("bug1: init done, %d, before binson_deserialize()\n", res);

    res = binson_deserialize(obj, parser, NULL, NULL, false);
    if (FAILED(res))
    {
      fprintf(stderr, "\nparser error: 0x%02x\n\n", res);
      return res;
    }
    // --> Segmentation fault (core dumped)

    printf("bug1: deserialized\n");    // We never get here!    

    printf("\n\n------  tree traversal   ---------\n");
    binson_traverse( obj, binson_get_root(obj), BINSON_TRAVERSE_PREORDER, BINSON_DEPTH_LIMIT, binson_cb_dump_debug, NULL );
    printf("---------------\n");
           
    
    binson_parser_free(parser);
    binson_io_free(err);
    binson_io_free(in);
    binson_free(obj);

    printf("bug1: SUCCESS\n");  
    
    
    return res;
}

/*================================================*/

/** Creates m1 PoT message in provided buffer. Returns actual size used. */
int32_t create_m1(uint8_t* buffer, int32_t buffer_size, uint8_t* ek, int32_t ek_size)
{
    binson *obj;
    binson_res res;
    binson_writer *writer;
    binson_io *out;
    uint32_t i;

    binson_raw_size  rsize=0;
    
    printf("Hello from create_m1, buffer_size: %d, ek_size: %d\n", buffer_size, ek_size);

    memset(buffer, 0, buffer_size);

    res = binson_new(&obj);
    res = binson_io_new(&out);
    res = binson_writer_new(&writer);

    res = binson_io_attach_bytebuf(out, buffer, buffer_size);
    res = binson_writer_init( writer, out, BINSON_WRITER_FORMAT_RAW);
    res = binson_init(obj, NULL);

    res = binson_node_add_str(obj, binson_get_root(obj), "t", NULL, "m1");
    res = binson_node_add_str(obj, binson_get_root(obj), "p", NULL, "S1");
    res = binson_node_add_bytes(obj, binson_get_root(obj), "ek", NULL, ek, ek_size);

    res = binson_serialize(obj, writer, &rsize);  // use NULL as second arg if not required
    printf("\n binson_serialize() : rsize: %d\n", rsize);    
    
    res = binson_writer_free(writer);
    res = binson_io_free(out);
    res = binson_free(obj);

    UNUSED(res);
    
    uint32_t last_index = -1;
    for (i = buffer_size - 1; i >= 0; i--)
    {
        if (buffer[i] != 0)
        {
            last_index = i;
            break;
        }
    }

    int32_t byte_size = last_index == -1 ? -1 : last_index + 1;
    printf("byte_size: %d\n", byte_size);
    return byte_size;
}
/*================================================*/

#define CK (printf("res=%d\n", res));
/**
 * Parses Binson bytes in buffer. Stores result in obj.
 * Caller is responsible for calling binson_free(obj) to free memory allocated in the function.
 */
int bn_read_from_bytes(uint8_t* buffer, int32_t buffer_size, binson** obj)
{
    binson_res res;
    binson_parser* parser;
    binson_io* in;

    res = binson_new(obj); CK
    res = binson_io_new(&in); CK
    res = binson_parser_new(&parser); CK
    res = binson_io_attach_bytebuf(in, buffer, buffer_size); CK
    res = binson_parser_init(parser, in, BINSON_PARSER_MODE_DOM); CK
    res = binson_init(*obj, NULL); CK
    res = binson_deserialize(*obj, parser, NULL, NULL, false); CK
    res = binson_parser_free(parser); CK
    res = binson_io_free(in); CK
    return 0;
}

/**
 * Writes a Binson object to a byte buffer. output_size is the actual
 * byte size used by the binson bytes.
 */
int bn_write_to_bytes(binson* obj, uint8_t* buffer, int32_t buffer_size, int32_t* output_size)
{
    binson_res res;
    binson_writer *writer;
    binson_io *out;
    binson_raw_size raw_size;

    res = binson_io_new(&out); CK
    res = binson_writer_new(&writer); CK
    res = binson_io_attach_bytebuf(out, buffer, buffer_size); CK
    res = binson_writer_init(writer, out, BINSON_WRITER_FORMAT_RAW); CK

    // XXX ERROR problematic with binson_init(), resets obj to empty Binson object.
    //res = binson_init(obj, writer, NULL, NULL); CK
    //res = binson_set_writer(obj, writer); CK
    
    res = binson_serialize(obj, writer, &raw_size); CK

    res = binson_writer_free(writer); CK
    res = binson_io_free(out); CK

    *output_size = raw_size;
    return 0;
}
/*================================================*/

int main()
{
  /*int r=0;
  uint8_t  buff[128];
  int32_t buff_size = 128;
  uint8_t ek[] = {0x10, 0x11, 0x12};
  int32_t ek_size = 3;
  */
  
  /*
  printf("\n--- before bug1() call\n");
  r = bug1();
  printf("\n--- after bug1() call\n");
  */
  
  /*printf("\n--- before create_m1() call\n");
  r = create_m1( buff, buff_size, ek, ek_size);
  printf("\n resulting raw size = %d bytes\n", r);   
  printf("\n--- after create_m1() call\n"); */
  
  binson_res res = BINSON_RES_OK;
  uint8_t buf[] = {0x40, 0x14, 0x01, 0x61, 0x10, 0x7b, 0x41};
  uint8_t buf2[32];
  int32_t i, bcnt = 0;
  binson *obj = NULL;
  
  printf("==== bn_read_from_bytes() begin.... ");
  res = bn_read_from_bytes(buf, sizeof(buf), &obj); CK;
  
  //binson_traverse( obj, binson_get_root(obj), BINSON_TRAVERSE_PREORDER, BINSON_DEPTH_LIMIT, binson_cb_dump_debug, NULL );

  printf("==== bn_read_from_bytes() end. ");
  printf("==== bn_write_to_bytes() begin.... ");
  res = bn_write_to_bytes(obj, buf2, sizeof(buf2), &bcnt); CK;
  printf("==== bn_write_to_bytes() end. ");
  
  printf("bcnt=%d\n\n", bcnt);
  for (i=0; i<bcnt; i++)
  {
    printf("0x%02x, ", buf2[i]);
  }
  printf("\n\n");
    
  res = binson_free(obj);
  
  return 0;
}
