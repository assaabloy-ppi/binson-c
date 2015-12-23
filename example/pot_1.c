/**
 *  PoT Sample 1
 */

#include <stdio.h>
#include "binson/binson.h"


/* generate reply to  */
void reply_to_msg( binson *rx, binson *tx )
{
  binson_node      *cid_node;
  binson_res        res;
  int64_t           cid;

  /* obrain 'cid' value from rx */
  res = binson_node_get_child_by_key( rx, NULL, "cid", &cid_node );
  res = binson_node_get_integer( cid_node, &cid );

  res = binson_node_add_integer( tx, binson_get_root(tx), "cid", NULL, cid);  /* use cid value from rx binson DOM */
  res = binson_node_add_object_empty( tx, binson_get_root(tx), "z", NULL);
}


int main()
{
    binson          *rx, *tx;
    binson_writer   *writer;
    binson_parser   *parser;

    binson_io       *err_io, *fin, *fout;
    binson_res       res;

    /* DEBUG: disable stdout buffering for debugging purposes */
    /*setvbuf(stdout, NULL, _IONBF, 0);*/

    res = binson_io_new( &err_io );
    res = binson_io_new( &fin );
    res = binson_io_new( &fout );

    res = binson_io_attach_stream( err_io, stdout );
    res = binson_io_open_file( fin, "./pot_1_in.bson", BINSON_IO_MODE_READ );
    res = binson_io_open_file( fout, "./pot_1_out.bson", BINSON_IO_MODE_WRITE | BINSON_IO_MODE_CREATE );

    res = binson_parser_new( &parser );
    res = binson_parser_init( parser, fin, BINSON_PARSER_MODE_DOM );

    res = binson_writer_new( &writer );
    res = binson_writer_init( writer, fout,  BINSON_WRITER_FORMAT_RAW );

    res = binson_new( &rx );
    res = binson_init( rx, NULL, parser, err_io );

    res = binson_new( &tx );
    res = binson_init( tx, writer, NULL, err_io );

    /* ready to build DOM */
    res = binson_deserialize( rx, NULL, NULL, false ); /* NULL mean replace whole DOM tree */

    /* place logic in this function */
    reply_to_msg( rx, tx );

    /* save to file as raw binson format */
    res = binson_serialize( tx );

    /* we are done. freeing resources */
    res = binson_free( rx );
    res = binson_free( tx );
    res = binson_parser_free( parser );
    res = binson_writer_free( writer );

    res = binson_io_free( err_io );
    res = binson_io_free( fin );
    res = binson_io_free( fout );

   return res;
}
