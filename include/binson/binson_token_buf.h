/*
 *  Copyright (c) 2015 ASSA ABLOY AB
 *
 *  This file is part of binson-c, BINSON serialization format library in C.
 *
 *  SPDX-License-Identifier: MIT
 *  See the LICENSE file in the project root for the full license text.
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