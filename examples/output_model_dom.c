/**
 *  This example demonstrates basic hi-level binson library API
 *  to build DOM model and generate binary output via 'binson_serialize()'
 */

#include "../src/binson.h"


int main()
{
    binson          *context;
    binson_io       *io;
    binson_res       res;

    res = binson_io_new( &io );
    res = binson_io_attach_stream( io, stdout );

    res = binson_new( &context );
    res = binson_init( context, NULL, NULL, io );

    binson_free( context );
    binson_io_free( io );

   return res;
}
