/**
 *  Template usefull for quick bugreport checks
 */

#include <stdio.h>
#include <string.h>

#include "binson/binson.h"


void reproduce_issue_160125()
{
    binson_res res;
    binson_parser* parser;
    binson_io* in;
    binson* obj;
    int32_t buffer_size = 2;
    uint8_t buffer[] = {0x40, 0x41};    /* empty Binson object */

    UNUSED(res);
    
    res = binson_new(&obj);
    res = binson_init(obj, NULL);

    printf("p10\n");

    res = binson_io_new(&in);
    res = binson_parser_new(&parser);
    res = binson_io_attach_bytebuf(in, buffer, buffer_size);
    res = binson_parser_init(parser, in, BINSON_PARSER_MODE_DOM);
    res = binson_deserialize(obj, parser, NULL, NULL, false);

    printf("p30\n");
    res = binson_parser_free(parser);
    res = binson_io_free(in);
    printf("p40\n");
    res = binson_free(obj);     // --> Segmentation fault
    printf("p50\n");
}

/*================================================*/

int main()
{
  
  reproduce_issue_160125();
  
  
  return 0;
}
