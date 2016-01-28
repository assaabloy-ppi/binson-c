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
 * \file binson_token_buf.h
 * \brief Token buffer (token level parsing) header
 *
 * \author Alexander Reshniuk
 * \date 11/12/2015
 *
 ***********************************************/

#ifndef BINSON_TOKEN_BUF_H_INCLUDED
#define BINSON_TOKEN_BUF_H_INCLUDED

#include "binson_config.h"
#include "binson_common.h"
#include "binson_error.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  Forward declarations
 */
typedef struct binson_token_buf_  binson_token_buf;

/*
 *  Token buffer API calls
 */
binson_res  binson_token_buf_new( binson_token_buf **ptbuf );
binson_res  binson_token_buf_init( binson_token_buf *tbuf, uint8_t *bptr, binson_raw_size bsize, binson_io *source );
binson_res  binson_token_buf_reset( binson_token_buf *tbuf );
binson_res  binson_token_buf_free( binson_token_buf *tbuf );

/* getters/setters */
binson_res  binson_token_buf_set_io( binson_token_buf *tbuf, binson_io *source );
binson_io*  binson_token_buf_get_io( binson_token_buf *tbuf  );
binson_res  binson_token_buf_get_buf( binson_token_buf *tbuf, uint8_t **pbptr, binson_raw_size *pbsize );
binson_res  binson_token_buf_set_buf( binson_token_buf *tbuf, uint8_t *bptr, binson_raw_size bsize );

binson_res  binson_token_buf_token_fill( binson_token_buf *tbuf, uint8_t *tok_count );
binson_res  binson_token_buf_get_token_payload( binson_token_buf *tbuf, uint8_t tok_num, binson_raw_value *raw_val );
binson_res  binson_token_buf_get_sig( binson_token_buf *tbuf, uint8_t tok_num, uint8_t *psig );

binson_res  binson_token_buf_get_node_type( binson_token_buf *tbuf, uint8_t tok_num, binson_node_type *pntype, bool *is_closing_token );

binson_res  binson_token_buf_is_partial( binson_token_buf *tbuf, bool *pbool );
binson_res  binson_token_buf_is_valid( binson_token_buf *tbuf, bool *pbool );

#ifdef __cplusplus
}
#endif

#endif /* BINSON_TOKEN_BUF_H_INCLUDED */