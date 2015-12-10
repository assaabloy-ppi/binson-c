/**
 *  This example demonstrates binson library debugging features
 */

#include <stdio.h>
#include "../src/binson.h"

void build_dom( binson *ctx, binson_node *root )
{
  binson_res       res = BINSON_RES_OK;
  binson_node      *n1, *n2, *n3;

  res = binson_node_add_object_empty( ctx, root, "key_1",  &n1);
  res = binson_node_add_object_empty( ctx, n1, "key_2",  &n2);
  res = binson_node_add_integer( ctx, n2, "key_3", &n3, 333);
  res = binson_node_add_boolean( ctx, n2, "key_4", &n3, true);
  res = binson_node_add_double( ctx, n2, "key_5", &n3, -3.1415);
}

int main()
{
    binson          *context;
    binson_writer   *writer;

    binson_io       *err_io, *out;
    binson_res       res;

    /* DEBUG: disable stdout buffering for debugging purposes */
    setvbuf(stdout, NULL, _IONBF, 0);

    res = binson_io_new( &err_io );
    res = binson_io_new( &out );
    res = binson_io_attach_stream( err_io, stdout );
    res = binson_io_attach_stream( out, stdout );

    res = binson_writer_new( &writer );
    res = binson_writer_init( writer, out, BINSON_WRITER_FORMAT_JSON_NICE );

    res = binson_new( &context );
    res = binson_init( context, writer, NULL, err_io );

    /* ready to build DOM */
    build_dom( context, binson_get_root( context ) );

    res = binson_serialize( context );

    /* DEBUG: test tree traversal */
#ifdef DEBUG
     printf("\n\n------ postorder tree traversal (e.g. to debug tree deletion)  ---------\n");
     binson_traverse( context, binson_get_root(context), BINSON_TRAVERSE_POSTORDER, BINSON_DEPTH_LIMIT, binson_cb_dump_debug, NULL );
     printf("---------------\n");
#endif

    /* we are done. freeing resources */
    res = binson_free( context );
    res = binson_writer_free( writer );
    res = binson_io_free( err_io );
    res = binson_io_free( out );

   return res;
}
