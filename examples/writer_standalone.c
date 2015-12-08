/**
 *  This example demonstrates 'binson_writer's low-level API usage to generate
 *  raw binson format output along with readable hex and JSON output
 */

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>

#include "../src/binson_writer.h"

void  sample_dump(binson_writer *writer)
{
  uint8_t buff[] = {0,1,2,3,4,0xff};

  binson_writer_write_array_begin( writer, NULL );

  binson_writer_write_object_begin( writer, NULL );
  binson_writer_write_str( writer, "key_1", "\u6DD8\u5B9D\u7F51" );  /* need UTF8 unescaping */
  binson_writer_write_object_begin( writer, "key_2" );
  binson_writer_write_double( writer, "key_3", -1.12345 );
  binson_writer_write_object_end( writer );
  binson_writer_write_object_end( writer );

  binson_writer_write_boolean( writer, NULL, true );
  binson_writer_write_boolean( writer, NULL, false );

  binson_writer_write_integer( writer, NULL, 1 );
  binson_writer_write_integer( writer, NULL, -1 );
  binson_writer_write_integer( writer, NULL, INT32_MAX );
  binson_writer_write_integer( writer, NULL, INT32_MIN );
  binson_writer_write_integer( writer, NULL, INT64_MAX );
  binson_writer_write_integer( writer, NULL, INT64_MIN );
  binson_writer_write_integer( writer, NULL, INT64_MAX / 2 );
  binson_writer_write_integer( writer, NULL, -(INT64_MAX / 2) );

  binson_writer_write_double( writer, NULL, 3.1415 );
  binson_writer_write_double( writer, NULL, 1.012e+45 );
  binson_writer_write_double( writer, NULL, NAN );
  binson_writer_write_double( writer, NULL, INFINITY );

  binson_writer_write_str( writer, NULL, "Simple ASCII text" );
  binson_writer_write_str( writer, NULL, "UTF8: приклад" );     /* normally UTF-8 encoded by compiler at build time */
  binson_writer_write_str( writer, NULL, "\xf6\xae" );          /* ASCII: ö® */

  binson_writer_write_bytes( writer, NULL, buff, 6 );
  binson_writer_write_array_end( writer );
}

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
     sample_dump( writer );
     printf("\n---------------------------------------------------------\n");
   }

   binson_writer_free( writer );
   binson_io_free( io );

   return 0;
}
