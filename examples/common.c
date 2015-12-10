
#include "common.h"

/* \brief
 *
 * \param ctx binson*
 * \param root binson_node*
 * \return void
 */
void  gen_sample_data_binson( binson *ctx, binson_node *root )
{
  binson_res       res;
  binson_node      *n1, *n2, *n3;

  res = binson_node_add_object_empty( ctx, root, "key_1",  &n1);
  res = binson_node_add_object_empty( ctx, n1, "key_2",  &n2);
  res = binson_node_add_integer( ctx, n2, "key_3", &n3, 333);
  res = binson_node_add_boolean( ctx, n2, "key_4", &n3, true);
  res = binson_node_add_double( ctx, n2, "key_5", &n3, -3.1415);
}

/* \brief
 *
 * \param writer binson_writer*
 * \return void
 */
void  gen_sample_data_binson_writer( binson_writer *writer )
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
