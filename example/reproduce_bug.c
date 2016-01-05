/**
 *  Template usefull for quick bugreport checks
 */

#include <stdio.h>
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
    res = binson_init(obj, NULL, parser, err);

    printf("bug1: init done, %d, before binson_deserialize()\n", res);

    res = binson_deserialize(obj, NULL, NULL, false);
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


int main()
{
  int r;
  
  printf("--- before bug1() call");
  r = bug1();
  printf("--- after bug1() call");
  
  return r;
}
