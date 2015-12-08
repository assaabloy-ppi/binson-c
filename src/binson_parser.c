/*
 *  Copyright (c) 2015 Contributors as noted in the AUTHORS file
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
#include "binson_parser.h"


typedef struct binson_parser_
{
  binson_io*            source;          /* Associated  'binson_io' struct */
  binson_parser_mode    mode;

  bool is_done;                      /* Parsing finished */

} binson_parser_;


/* \brief Validates binson signature
 *
 * \param byte uint8_t
 * \return bool
 */
bool is_sig( uint8_t byte )
{
  return true;
}


/* \brief
 *
 * \param pparser binson_parser**
 * \return binson_res
 */
binson_res  binson_parser_new( binson_parser **pparser )
{
  /* Initial parameter validation */
  if (!pparser )
    return BINSON_RES_ERROR_ARG_WRONG;

  *pparser = (binson_parser *)malloc(sizeof(binson_parser_));

  return BINSON_RES_OK;
}

/* \brief
 *
 * \param parser binson_parser*
 * \param source binson_io*
 * \param mode binson_parser_mode
 * \return binson_res
 */
binson_res  binson_parser_init( binson_parser *parser, binson_io *source, binson_parser_mode mode )
{
  /* Initial parameter validation */
  if (!parser || !source || mode < 0 || mode >= BINSON_PARSER_MODE_LAST )
    return BINSON_RES_ERROR_ARG_WRONG;

  parser->source    = source;
  parser->mode  = mode;

  return BINSON_RES_OK;
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
  if (!parser || mode < 0 || mode >= BINSON_PARSER_MODE_LAST)
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

}

/* \brief
 *
 * \param parser binson_parser*
 * \return binson_res
 */
binson_res  binson_parser_parse_next( binson_parser *parser )
{

}

/* \brief
 *
 * \param parser binson_parser*
 * \return bool
 */
bool  binson_parser_is_done( binson_parser *parser )
{
  return parser->is_done;
}

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
binson_res binson_token_parse_boolean( binson_parser *parser, binson_token *token, bool *val )
{

}

/* \brief
 *
 * \param parser binson_parser*
 * \param token binson_token*
 * \param val int64_t*
 * \return binson_res
 */
binson_res token_parse_integer( binson_parser *parser, binson_token *token, int64_t *val )
{

}

/* \brief
 *
 * \param parser binson_parser*
 * \param token binson_token*
 * \param val double*
 * \return binson_res
 */
binson_res token_parse_double( binson_parser *parser, binson_token *token, double *val )
{

}

/* \brief
 *
 * \param parser binson_parser*
 * \param token binson_token*
 * \param val char*
 * \return binson_res
 */
binson_res token_parse_str( binson_parser *parser, binson_token *token, char *val )
{

}

/* \brief
 *
 * \param parser binson_parser*
 * \param token binson_token*
 * \param bptr uint8_t*
 * \param bsize size_t*
 * \return binson_res
 */
binson_res token_parse_bytes( binson_parser *parser, binson_token *token, uint8_t *bptr, size_t *bsize )
{

}
