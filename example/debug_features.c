/**
 *  This example demonstrates binson library debugging features
 */

#include <stdio.h>
#include "binson/binson.h"
#include "common.h"

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
    res = binson_writer_init( writer, out, BINSON_WRITER_FORMAT_HEX );

    res = binson_new( &context );
    res = binson_init( context, writer, NULL, err_io );

    /* ready to build DOM */
    gen_sample_data_binson( context, binson_get_root( context ));

    res = binson_serialize( context, NULL );

    /* DEBUG: test tree traversal */
#ifdef DEBUG
     printf("\n\n------ postorder tree traversal (e.g. to debug tree deletion)  ---------\n");
     binson_traverse( context, binson_get_root(context), BINSON_TRAVERSE_PREORDER, BINSON_DEPTH_LIMIT, binson_cb_dump_debug, NULL );
     printf("---------------\n");
#endif

    /* we are done. freeing resources */
    res = binson_free( context );
    res = binson_writer_free( writer );
    res = binson_io_free( err_io );
    res = binson_io_free( out );

   return res;
}
