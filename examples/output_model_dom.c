/**
 *  This example demonstrates basic hi-level binson library API
 *  to build DOM model and generate binary output via 'binson_serialize()'
 */

#include <stdio.h>
#include "../src/binson.h"

void build_dom( binson *ctx, binson_node *root )
{
  binson_res       res;
  binson_node      *n1, *n2, *n3;

  res = binson_node_add_object_empty( ctx, root, "key_1",  &n1);
  res = binson_node_add_object_empty( ctx, n1, "key_2",  &n2);
  res = binson_node_add_integer( ctx, n2, "key_3", &n3, 333);
  res = binson_node_add_boolean( ctx, n2, "key_4", &n3, true);
  res = binson_node_add_double( ctx, n2, "key_5", &n3, -3.1415);

  /*res = binson_node_add_str( ctx, root, "key_2",  0, "sample_string");*/

/*  res = binson_node_add_integer( ctx, root, "key_3", &n3, 333); */

}

int main()
{
    binson          *context;
    binson_writer   *writer;
    binson_parser   *parser;

    binson_io       *err_io, *in, *out;
    binson_res       res;

    /* disable stdout buffering for debugging purposes */
    /*setvbuf(stdout, NULL, _IONBF, 0);*/

    res = binson_io_new( &err_io );
    res = binson_io_new( &in );
    res = binson_io_new( &out );

    res = binson_io_attach_stream( err_io, stdout );
    res = binson_io_attach_stream( in, stdin );
    res = binson_io_attach_stream( out, stdout );

    res = binson_writer_new( &writer );
    res = binson_writer_init( writer, out, BINSON_WRITER_FORMAT_JSON_NICE );

    res = binson_parser_new( &parser );
    res = binson_parser_init( parser, in, BINSON_PARSER_MODE_DOM );

    res = binson_new( &context );
    res = binson_init( context, writer, parser, err_io );

    /* ready to build DOM */
    build_dom( context, binson_get_root( context ) );

    /* serialize via attached 'binson_writer' */
    res = binson_serialize( context );

    /* we are done. freeing resources */
    res = binson_parser_free( parser );
    res = binson_writer_free( writer );

    binson_free( context );
    binson_io_free( err_io );
    binson_io_free( in );
    binson_io_free( out );

   return res;
}
