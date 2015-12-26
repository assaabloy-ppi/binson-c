/**
 *  This example demonstrates 'binson_writer's low-level API usage to generate
 *  raw binson format output along with readable hex and JSON output
 */

#include <stdio.h>
#include <stdlib.h>
#include <float.h>

#include "binson/binson_writer.h"
#include "common.h"

int main()
{
    binson_res             res;
    binson_io              *io;
    binson_writer          *writer;
    binson_writer_format    f;

    res = binson_io_new( &io );
    res = binson_io_attach_stream( io, stdout );

    res = binson_writer_new( &writer );
    res = binson_writer_init( writer, io, BINSON_WRITER_FORMAT_RAW );


   for (f = BINSON_WRITER_FORMAT_RAW; f<BINSON_WRITER_FORMAT_LAST; f++)
   {
     binson_writer_set_format( writer, f );
     gen_sample_data_binson_writer( writer );
     printf("\n---------------------------------------------------------\n");
   }

   binson_writer_free( writer );
   binson_io_free( io );

   return res;
}
