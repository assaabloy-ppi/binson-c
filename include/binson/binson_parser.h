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
 * \file binson_parser.h
 * \brief Binson binary format parsing API header file
 *
 * \author Alexander Reshniuk
 * \date 20/11/2015
 *
 ***********************************************/

#ifndef BINSON_PARSER_H_INCLUDED
#define BINSON_PARSER_H_INCLUDED

#include "binson_config.h"
#include "binson_common.h"
#include "binson_error.h"
#include "binson_io.h"
#include "binson_token_buf.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 *  Forward declarations
 */
typedef struct binson_parser_     binson_parser;
typedef struct binson_token_ref_  binson_token_ref;

/**
 *  Binson parser mode enum type
 */
typedef enum {
  BINSON_PARSER_MODE_RAW    = 0,        /**< Raw mode. Serialized binson raw data used as underlying storage  */
  BINSON_PARSER_MODE_SMART,             /**< Raw mode + some caching */
  BINSON_PARSER_MODE_DOM,               /**< Full DOM creation. Raw data not stored but reconstructed on binson_serialize() call */

  BINSON_PARSER_MODE_LAST               /**< Enum terminator. Need for arg validation */

} binson_parser_mode;


/**
 *  Parsing callback declaration
 */
typedef binson_res (*binson_parser_cb)( binson_parser *parser, uint8_t token_cnt, binson_token_buf *tbuf, void* param );

/**
 *  Binson parser API calls
 */
binson_res  binson_parser_new( binson_parser **pparser );
binson_res  binson_parser_init( binson_parser *parser, binson_io *source, binson_parser_mode mode );
binson_res  binson_parser_reset( binson_parser *parser );
binson_res  binson_parser_free( binson_parser *parser );
binson_res  binson_parser_set_io( binson_parser *parser, binson_io *source );
binson_io*  binson_parser_get_io( binson_parser *parser );
binson_res  binson_parser_set_mode( binson_parser *parser, binson_parser_mode mode );

binson_res  binson_parser_parse( binson_parser *parser, binson_parser_cb cb, void* param );
binson_res  binson_parser_parse_first( binson_parser *parser, binson_parser_cb cb, void* param );
binson_res  binson_parser_parse_next( binson_parser *parser );
bool        binson_parser_is_done( binson_parser *parser );
bool        binson_parser_is_valid( binson_parser *parser );

bool        binson_parser_copy_token_to_( binson_parser *parser, binson_token_ref *token );

/*binson_res  binson_parser_validate( binson_parser *parser );*/


/* binson_res        binson_token_parse( binson_parser *parser, binson_token *token, binson_value *val );*/



#ifdef __cplusplus
}
#endif

#endif /* BINSON_PARSER_H_INCLUDED */
