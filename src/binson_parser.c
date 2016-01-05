/*
 *  Copyright (c) 2015 ASSA ABLOY AB
 *
 *  This file is part of binson-c, BINSON serialization format library in C.
 *
 *  binson-c is free software; you can redistribute it and/or modify it under
 *  the terms of the GNU Lesser General Public License (LGPL) as published
 *  by the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  As a special exception, the Contributors give you permission to link
 *  this library with independent modules to produce an executable,
 *  regardless of the license terms of these independent modules, and to
 *  copy and distribute the resulting executable under terms of your choice,
 *  provided that you also meet, for each linked independent module, the
 *  terms and conditions of the license of that module. An independent
 *  module is a module which is not derived from or based on this library.
 *  If you modify this library, you must extend this exception to your
 *  version of the library.
 *
 *  binson-c is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 *  License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/********************************************//**
 * \file binson_parser.c
 * \brief Binson binary format parsing API implementation file
 *
 * \author Alexander Reshniuk
 * \date 20/11/2015
 *
 ***********************************************/

#include <string.h>
#include <stdlib.h>

#include "binson_config.h"
#include "binson_common_pvt.h"
#include "binson/binson_parser.h"
#include "binson_token_buf.h"

/**
 *  Parser context
 */
typedef struct binson_parser_
{
  binson_io            *source;
  binson_parser_mode    mode;

  binson_token_buf     *token_buf;

  /* store status data between iterations */
  binson_parser_cb      cb;
  void*                 param;

  bool                  sig_stack[BINSON_DEPTH_LIMIT];  /* used to decide do we need to request key-val pair or just val */
  binson_depth          depth;

  bool                  done;                      /* Parsing finished */
  bool                  valid;                     /* false if something in raw input was violate binson specs */


} binson_parser_;

/* \brief
 *
 * \param pparser binson_parser**
 * \return binson_res
 */
binson_res  binson_parser_new( binson_parser **pparser )
{
  binson_res  res;

  /* Initial parameter validation */
  if (!pparser )
    return BINSON_RES_ERROR_ARG_WRONG;

  *pparser = (binson_parser *)malloc(sizeof(binson_parser_));
  if (!*pparser)
    return BINSON_RES_ERROR_OUT_OF_MEMORY;

  res =  binson_token_buf_new( &((*pparser)->token_buf) );

  if (!(*pparser)->token_buf)
    return BINSON_RES_ERROR_OUT_OF_MEMORY;
  else
    if (FAILED(res)) return res;     

  return BINSON_RES_OK;
}

/*8 \brief
 *
 * \param parser binson_parser*
 * \param source binson_io*
 * \param mode binson_parser_mode
 * \return binson_res
 */
binson_res  binson_parser_init( binson_parser *parser, binson_io *source, binson_parser_mode mode )
{
  binson_res  res;

  /* Initial parameter validation */
  if (!parser || !source || mode >= BINSON_PARSER_MODE_LAST )
    return BINSON_RES_ERROR_ARG_WRONG;

  parser->source  = source;
  parser->mode    = mode;

  parser->done              = false;
  parser->valid             = true;
  parser->depth             = 0;

  res = binson_token_buf_init( parser->token_buf, NULL, 0, parser->source );

  return res;
}

/** \brief Reset parser after previous invalid parsing session
 *
 * \param parser binson_parser*
 * \return binson_res
 */
binson_res  binson_parser_reset( binson_parser *parser )
{
  return binson_parser_init( parser, parser->source, parser->mode );
}

/* \brief
 *
 * \param parser binson_parser*
 * \return binson_res
 */
binson_res  binson_parser_free( binson_parser *parser )
{
  /* Initial parameter validation */
  if (!parser)
    return BINSON_RES_ERROR_ARG_WRONG;

  if (parser->token_buf)
    binson_token_buf_free( parser->token_buf );

  if (parser)
    free( parser );

  return BINSON_RES_OK;
}

/* \brief
 *
 * \param parser binson_parser*
 * \param source binson_io*
 * \return binson_res
 */
binson_res  binson_parser_set_io( binson_parser *parser, binson_io *source )
{
  /* Initial parameter validation */
  if (!parser || !source)
    return BINSON_RES_ERROR_ARG_WRONG;

  parser->source = source;

  return BINSON_RES_OK;
}

/* \brief
 *
 * \param parser binson_parser*
 * \param mode binson_parser_mode
 * \return binson_res
 */
binson_res  binson_parser_set_mode( binson_parser *parser, binson_parser_mode mode )
{
  ASSERT_STATIC( BINSON_PARSER_MODE_LAST > 0 );   /* At least one of 'BINSON_PARSER_MODE_*' must be defined */

  /* Initial parameter validation */
  if (!parser || mode >= BINSON_PARSER_MODE_LAST)
    return BINSON_RES_ERROR_ARG_WRONG;

  /* Also check is lib built with feature or not */
#ifndef WITH_BINSON_PARSER_MODE_RAW
  if (mode == BINSON_PARSER_MODE_RAW) return  BINSON_RES_ERROR_NOT_SUPPORTED;
#endif
#ifndef WITH_BINSON_PARSER_MODE_SMART
  if (mode == BINSON_PARSER_MODE_SMART) return  BINSON_RES_ERROR_NOT_SUPPORTED;
#endif
#ifndef WITH_BINSON_PARSER_MODE_DOM
  if (mode == BINSON_PARSER_MODE_DOM) return  BINSON_RES_ERROR_NOT_SUPPORTED;
#endif

  parser->mode = mode;

  return BINSON_RES_OK;
}

/* \brief
 *
 * \param parser binson_parser*
 * \param cb binson_parser_cb
 * \param param void*
 * \return binson_res
 */
binson_res  binson_parser_parse( binson_parser *parser, binson_parser_cb cb, void* param )
{
  binson_res  res;

  res = binson_parser_parse_first( parser, cb, param );  /* Request single token at first stage of parsing */
  
  while (SUCCESS(res) && !parser->done && parser->valid )
    res = binson_parser_parse_next( parser );

  return res;
}

/* \brief
 *
 * \param parser binson_parser*
 * \param cb binson_parser_cb
 * \param param void*
 * \return binson_res
 */
binson_res  binson_parser_parse_first( binson_parser *parser, binson_parser_cb cb, void* param )
{
  binson_res  res;
  uint8_t     tok_request, sig;
  bool        valid, partial;

  if (!parser)
    return BINSON_RES_ERROR_ARG_WRONG;

  if (cb)
  {
    parser->cb                = cb;
    parser->param             = param;
    parser->depth             = 0;
  }

  res = binson_token_buf_reset( parser->token_buf );  /* make sure token buffer is empty */

  tok_request = parser->depth? (parser->sig_stack[parser->depth-1] == BINSON_SIG_OBJ_BEGIN? 2:1) : 1;

  res = binson_token_buf_token_fill( parser->token_buf, &tok_request );
  if (FAILED(res)) return res;
  
  res = binson_token_buf_is_valid( parser->token_buf, &valid );

  if (!valid)
    return BINSON_RES_ERROR_PARSE_INVALID_INPUT;

  res = binson_token_buf_is_partial( parser->token_buf, &partial );

  if (!partial)  /* ??? */
    return BINSON_RES_ERROR_PARSE_PART;

  res = binson_token_buf_get_sig( parser->token_buf, tok_request-1 , &sig );  /* request signature for value part of key-value pair */

  switch (sig)
  {
    case BINSON_SIG_OBJ_BEGIN:
    case BINSON_SIG_ARRAY_BEGIN:
      parser->sig_stack[ parser->depth ] = sig;
      parser->depth++;
    break;

    case BINSON_SIG_OBJ_END:
    case BINSON_SIG_ARRAY_END:
      parser->depth--;
      if (!parser->depth)
        parser->done = true;
    break;

    default:
    break;
  }

  res = parser->cb( parser, tok_request, parser->token_buf, parser->param );

  return res;
}

/* \brief
 *
 * \param parser binson_parser*
 * \return binson_res
 */
binson_res  binson_parser_parse_next( binson_parser *parser )
{
  return binson_parser_parse_first( parser, NULL, NULL );
}

/** \brief
 *
 * \param parser binson_parser*
 * \return bool
 */
bool  binson_parser_is_done( binson_parser *parser )
{
  return parser->done;
}

/** \brief
 *
 * \param parser binson_parser*
 * \return bool
 */
bool binson_parser_is_valid( binson_parser *parser )
{
  return parser->valid;
}

/* \brief Input validation callback.
 *
 * \param parser binson_parser*
 * \param token binson_token*
 * \param param void*
 * \return binson_res
 */
/*binson_res  binson_parser_cb_validate( binson_parser *parser, binson_token *token, void* param )
{

}
*/
/* \brief
 *
 * \param parser binson_parser*
 * \param token binson_token*
 * \param val binson_value*
 * \return binson_res
 */
/*binson_res  binson_token_parse( binson_parser *parser, binson_token *token, binson_value *val )
{
  / Initial parameter validation /
  if (!parser || !token || !val)
    return BINSON_RES_ERROR_ARG_WRONG;

  switch (node_type)
  {
    case BINSON_TYPE_OBJECT:
    case BINSON_TYPE_ARRAY:
      return BINSON_RES_OK;

    case BINSON_TYPE_BOOLEAN:
      return binson_token_parse_boolean( parser, token, val->bool_val );

    case BINSON_TYPE_INTEGER:
      return binson_token_parse_integer( parser, token, val->int_val );

    case BINSON_TYPE_DOUBLE:
      return binson_token_parse_double( parser, token, val->double_val );

    case BINSON_TYPE_STRING:
      return binson_token_parse_string( parser, token, val->vector_val.vptr );

    case BINSON_TYPE_BYTES:
      return binson_token_parse_bytes( parser, token, val->vector_val.vptr, val->vector_val.vsize );

    case BINSON_TYPE_UNKNOWN:
    default:
      return BINSON_RES_ERROR_ARG_WRONG;
  }
}*/

/* \brief
 *
 * \param parser binson_parser*
 * \param token binson_token*
 * \param val bool*
 * \return binson_res
 */
/*binson_res binson_token_parse_boolean( binson_parser *parser, binson_token *token, bool *val )
{

}*/

/* \brief
 *
 * \param parser binson_parser*
 * \param token binson_token*
 * \param val int64_t*
 * \return binson_res
 */
/*binson_res token_parse_integer( binson_parser *parser, binson_token *token, int64_t *val )
{

}*/

/* \brief
 *
 * \param parser binson_parser*
 * \param token binson_token*
 * \param val double*
 * \return binson_res
 */
/*binson_res token_parse_double( binson_parser *parser, binson_token *token, double *val )
{

}*/

/* \brief
 *
 * \param parser binson_parser*
 * \param token binson_token*
 * \param val char*
 * \return binson_res
 */
/*binson_res token_parse_str( binson_parser *parser, binson_token *token, char *val )
{

}*/

/* \brief
 *
 * \param parser binson_parser*
 * \param token binson_token*
 * \param bptr uint8_t*
 * \param bsize size_t*
 * \return binson_res
 */
/*binson_res token_parse_bytes( binson_parser *parser, binson_token *token, uint8_t *bptr, size_t *bsize )
{

}*/
