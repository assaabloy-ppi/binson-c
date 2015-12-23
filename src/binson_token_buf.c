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
 * \file binson_token_buf.c
 * \brief Binson token buffer implementation file
 *
 * \author Alexander Reshniuk
 * \date 11/12/2015
 *
 ***********************************************/

#include <stdlib.h>
#include <string.h>

#include "binson_config.h"
#include "binson_common_pvt.h"
#include "binson/binson_common.h"
#include "binson_util.h"
#include "binson/binson_io.h"

/**
 *  Individual token info
 */
typedef struct binson_token_ref
{
  binson_token_type      type;
  binson_raw_size        offset;      /* offset from buffer begin */
  binson_raw_size        size;        /* how many bytes of token data already obtained  */

  binson_raw_size        len_size;    /* how many bytes takes length field of the token */
  binson_raw_size        val_size;    /* payload part size */

  bool                   is_partial;

} binson_token_ref;

/**
 *  Binson tokern buffer structure
 */
typedef struct binson_token_buf_
{
  /* data source */
  binson_io             *source;       /* Token buffer is smart enought to read from source in streaming mode */

  /* buffer related */
  uint8_t               *ptr;
  binson_raw_size        size;
  bool                   malloced;

  /* content related */
  binson_token_ref       tokens[BINSON_TOKEN_BUF_TOKS];
  uint8_t                current_token;
  uint8_t                tokens_requested;  /* how many tokens to obtain before returning results to caller */

  bool                   is_valid;

} binson_token_buf;


/*
 *  Forward declarations
 */
binson_res  binson_token_buf_set_buf( binson_token_buf *tbuf, uint8_t *bptr, binson_raw_size bsize );


bool  last_token_is_final( binson_token_buf *tbuf,  uint8_t tokens_requested )
{
  return (tbuf->current_token >= tokens_requested)? true : false;
}

/* \brief
 *
 * \param tbuf binson_token_buf*
 * \param missing_bytes binson_raw_size*
 * \param valid bool*
 * \return binson_res
 */
binson_res  last_token_rescan( binson_token_buf *tbuf, size_t *missing_bytes, bool *valid )
{
  binson_token_ref  *tok;
  /*uint8_t            cnt;*/
  int64_t            payload_len;

  if (!tbuf || !missing_bytes || !valid)
    return BINSON_RES_ERROR_ARG_WRONG;

  *valid = true;
  tok = &tbuf->tokens[ tbuf->current_token ];

  if (tok->size == 0)  /* have no data for token - need to obtain at least signature */
  {
    *missing_bytes = BINSON_RAW_SIG_SIZE;  /* size of signature according to BINSON specs */
    tok->is_partial = true;
    return BINSON_RES_ERROR_PARSE_PART;
  }

  /* at this point signature must present */
  switch ( *(tbuf->ptr + tok->offset) )
  {
    case BINSON_SIG_OBJ_END:
    case BINSON_SIG_ARRAY_END:
        tbuf->tokens_requested = 1;   /* force single token request because end signatures never have keys */
        /* not a break, continue execution */
    case BINSON_SIG_OBJ_BEGIN:
    case BINSON_SIG_ARRAY_BEGIN:
    case BINSON_SIG_TRUE:
    case BINSON_SIG_FALSE:
        tok->len_size = 0;
        tok->val_size = 0;
      /*cnt = BINSON_RAW_SIG_SIZE;*/
    break;

    case BINSON_SIG_INTEGER_8:
        tok->len_size = 0;
        tok->val_size = 1;
    break;

    case BINSON_SIG_STRING_8:
    case BINSON_SIG_BYTES_8:
        tok->len_size = 1;
        tok->val_size = 0;
      /*cnt = BINSON_RAW_SIG_SIZE + 1;*/
    break;

    case BINSON_SIG_INTEGER_16:
        tok->len_size = 0;
        tok->val_size = 2;
    break;

    case BINSON_SIG_STRING_16:
    case BINSON_SIG_BYTES_16:
        tok->len_size = 2;
        tok->val_size = 0;
      /*cnt = BINSON_RAW_SIG_SIZE + 2;*/
    break;

    case BINSON_SIG_INTEGER_32:
        tok->len_size = 0;
        tok->val_size = 4;
    break;

    case BINSON_SIG_STRING_32:
    case BINSON_SIG_BYTES_32:
      /*cnt = BINSON_RAW_SIG_SIZE + 4;*/
        tok->len_size = 4;
        tok->val_size = 0;
    break;

    case BINSON_SIG_DOUBLE:
    case BINSON_SIG_INTEGER_64:
      /*cnt = BINSON_RAW_SIG_SIZE + 8;*/
        tok->len_size = 0;
        tok->val_size = 8;
    break;

    default:
      *valid = false;
       return BINSON_RES_ERROR_PARSE_INVALID_INPUT;
  }

  /*tok->len_size = cnt - BINSON_RAW_SIG_SIZE;*/

  if (!tok->val_size && tok->size < BINSON_RAW_SIG_SIZE + tok->len_size)  /* missing part of length data */
  {
    *missing_bytes = BINSON_RAW_SIG_SIZE + tok->len_size - tok->size;
    tok->is_partial = true;
    return BINSON_RES_ERROR_PARSE_PART;
  }

  if ( tok->len_size ) /* token with length filed: STRING or BYTES */
  {
    /* at this point length data are ok - let's decode it */
    payload_len =  binson_util_unpack_integer( tbuf->ptr + tok->offset + BINSON_RAW_SIG_SIZE, tok->len_size  );
    tok->val_size = payload_len;

    /* calculate missing part of payload */
     *missing_bytes = BINSON_RAW_SIG_SIZE + tok->len_size + payload_len - tok->size;

     if (*missing_bytes == 0)
        tok->is_partial = false;
  }
  else if ( tok->val_size ) /* token without length field, but payload length implicitly encoded in signature */
  {
    /* calculate missing part of payload */
     *missing_bytes = BINSON_RAW_SIG_SIZE + tok->val_size - tok->size;

     if (*missing_bytes == 0)
        tok->is_partial = false;
  }
  else  /* looks like single byte token */
  {
    tok->val_size = 0;
    tok->is_partial = false;
    *missing_bytes = 0;
  }

  return *missing_bytes? BINSON_RES_ERROR_PARSE_PART : BINSON_RES_OK;
}



/** \brief
 *
 * \param ptbuf binson_token_buf**
 * \return binson_res
 */
binson_res  binson_token_buf_new( binson_token_buf **ptbuf )
{
  if (!ptbuf)
    return BINSON_RES_ERROR_ARG_WRONG;

  *ptbuf = (binson_token_buf *)calloc(sizeof(binson_token_buf), 1);

  return BINSON_RES_OK;
}

/** \brief
 *
 * \param tbuf binson_token_buf*
 * \return binson_res
 *
 */
binson_res  binson_token_buf_reset( binson_token_buf *tbuf )
{
  if (!tbuf)
    return BINSON_RES_ERROR_ARG_WRONG;

  tbuf->current_token = 0;
  tbuf->tokens_requested = 0;

  memset( &tbuf->tokens[tbuf->current_token], 0, sizeof(binson_token_ref) );

  tbuf->tokens[tbuf->current_token].is_partial = true;  /* token which has no signature is partial */
  tbuf->is_valid = true;


  return BINSON_RES_OK;
}

/** \brief Initialize context and allocates new buffer or alternatively use external buffer
 *
 * \param tbuf binson_token_buf*    Context
 * \param bptr uint8_t*             Pointer to external buffer. Set to NULL to use internal allocation
 * \param bsize binson_raw_size     Initial token buffer size. Set to 0 to use preconfigured buffer size
 * \param source binson_io*         Data source io instanse
 * \return binson_res               Result code
 */
binson_res  binson_token_buf_init( binson_token_buf *tbuf, uint8_t *bptr, binson_raw_size bsize, binson_io *source )
{
  binson_res  res;

  res = binson_token_buf_set_buf( tbuf, bptr, bsize );  /* set or allocate if needed */
  res = binson_token_buf_reset( tbuf );                 /* make it empty */

  tbuf->source = source;

  return res;
}

/** \brief
 *
 * \param tbuf binson_token_buf*
 * \return binson_res
 *
 */
binson_res  binson_token_buf_free( binson_token_buf *tbuf )
{
  if (!tbuf)
    return BINSON_RES_ERROR_ARG_WRONG;

  if (tbuf->malloced && tbuf->ptr)
    free( tbuf->ptr );

  if (tbuf)
    free( tbuf );

  return BINSON_RES_OK;
}

/** \brief
 *
 * \param tbuf binson_token_buf*
 * \param source binson_io*
 * \return binson_res
 *
 */
binson_res  binson_token_buf_set_io( binson_token_buf *tbuf, binson_io *source )
{
  if (!tbuf || !source)
    return BINSON_RES_ERROR_ARG_WRONG;

  tbuf->source = source;

  return BINSON_RES_OK;
}

/** \brief
 *
 * \param tbuf binson_token_buf*
 * \param pbptr uint8_t**
 * \param pbsize binson_raw_size*
 * \return binson_res
 *
 */
binson_res  binson_token_buf_get_buf( binson_token_buf *tbuf, uint8_t **pbptr, binson_raw_size *pbsize )
{
  if (!tbuf || !pbptr || !pbsize)
    return BINSON_RES_ERROR_ARG_WRONG;

  *pbptr  = tbuf->ptr;
  *pbsize = tbuf->size;

  return BINSON_RES_OK;
}

/** \brief
 *
 * \param tbuf binson_token_buf*
 * \param bptr uint8_t*
 * \param bsize binson_raw_size
 * \return binson_res
 *
 */
binson_res  binson_token_buf_set_buf( binson_token_buf *tbuf, uint8_t *bptr, binson_raw_size bsize )
{
  if (!tbuf)
    return BINSON_RES_ERROR_ARG_WRONG;

  if (tbuf->ptr && tbuf->malloced && !bptr && bsize > tbuf->size)  /* try to reallocate to bigger memory block */
  {
    uint8_t *pnew = (uint8_t *)realloc(tbuf->ptr, bsize);
    if (pnew)
    {
      tbuf->ptr   = pnew;
      tbuf->size  = bsize;
      return BINSON_RES_OK;
    }
    else
      return BINSON_RES_ERROR_OUT_OF_MEMORY;
  }

  if (tbuf->ptr && tbuf->ptr != bptr && tbuf->malloced)   /* looks like already allocated */
  {
    free( tbuf->ptr );
    tbuf->malloced  = false;
    tbuf->ptr       = NULL;
    tbuf->size      = 0;
  }

  if (!bsize)  /* if bsize is zero use predefined token buffer initial size */
    bsize = BINSON_TOKEN_BUF_SIZE;

  if (!bptr)  /* allocate buffer of specified size */
  {
     tbuf->ptr        = (uint8_t *)malloc(bsize);
     tbuf->size       = bsize;
     tbuf->malloced   = true;
  }
  else  /* just use external buffer as specified by args */
  {
    tbuf->ptr       = bptr;
    tbuf->size      = bsize;
    tbuf->malloced  = false;
  }

  return BINSON_RES_OK;
}

/** \brief Read data from source io till \c tok_count tokens become valid. Subsequent calls
 *  to this function continue token filling. It's used for streaming when underlying io layer
 *  can't fulfill one-time request
 *
 * \param tbuf binson_token_buf*
 * \param tok_count uint8_t           Number of valid tokens in buffer to return BINSON_RES_OK
 * \return binson_res
 *
 */
binson_res  binson_token_buf_token_fill( binson_token_buf *tbuf, uint8_t *tok_count )
{
  binson_token_ref    *tok;
  binson_res          res = BINSON_RES_OK;
  size_t              to_read, done_read;
  bool                valid = true;

  if (!tbuf || !*tok_count || *tok_count > BINSON_TOKEN_BUF_TOKS)
    return BINSON_RES_ERROR_ARG_WRONG;

  tbuf->tokens_requested = *tok_count;
  tok = &tbuf->tokens[ tbuf->current_token ];

  while (tbuf->current_token < tbuf->tokens_requested  && valid)
  {
      res = last_token_rescan(tbuf, &to_read, &valid);
      switch (res)
      {
        case BINSON_RES_OK:
            tbuf->current_token++;
            if (tbuf->current_token < BINSON_TOKEN_BUF_TOKS)  /* make sure we don't access data outsize tbuf->tokens[] */
            {
              tok = &tbuf->tokens[ tbuf->current_token ];
              tok->size = 0;
              tok->offset = tbuf->tokens[ tbuf->current_token-1 ].size;
            }
            continue;

        case BINSON_RES_ERROR_PARSE_PART:
          if (tbuf->size < tok->offset + tok->size + to_read) /* if buffer is too small try to reallocate to bigger one */
          {
            binson_raw_size   delta = MAX( tok->offset + tok->size + to_read - tbuf->size, BINSON_TOKEN_BUF_SIZE_INC );

            res = binson_token_buf_set_buf( tbuf, NULL, tbuf->size + delta );
            if (FAILED(res)) return res;  /* critical error */
          }
          res = binson_io_read( tbuf->source, tbuf->ptr + tok->offset + tok->size, to_read, &done_read );
          tok->size += done_read;
          continue;

        case BINSON_RES_ERROR_PARSE_INVALID_INPUT:
        default:
          valid = false;
          break;
      }
  }

  *tok_count = tbuf->tokens_requested;

  tbuf->is_valid = valid;
  tbuf->tokens_requested = 0;

  return res;
}

/** \brief
 *
 * \param tbuf binson_token_buf*
 * \param tok_num uint8_t
 * \param pttype binson_token_type*
 * \param is_closing_token bool*      returns true, if token is closing part of OBJECT/ARRAY
 * \return binson_res
 */
/*binson_res  binson_token_buf_get_token_type( binson_token_buf *tbuf, uint8_t tok_num, binson_token_type *pttype  )
{
  if (!tbuf || tok_num >= BINSON_TOKEN_BUF_TOKS || !pttype)
    return BINSON_RES_ERROR_ARG_WRONG;

  *pttype = *(tbuf->ptr + tbuf->tokens[ tok_num ].offset);

  return BINSON_RES_OK;
}
*/
/** \brief
 *
 * \param tbuf binson_token_buf*
 * \param tok_num uint8_t
 * \param pntype binson_node_type*
 * \param is_closing_token bool*
 * \return binson_res
 */
binson_res  binson_token_buf_get_node_type( binson_token_buf *tbuf, uint8_t tok_num, binson_node_type *pntype, bool *is_closing_token )
{
  binson_res  res;

  if (!tbuf || tok_num >= BINSON_TOKEN_BUF_TOKS || !pntype)
    return BINSON_RES_ERROR_ARG_WRONG;

  *pntype = binson_common_map_sig_to_node_type( *(tbuf->ptr + tbuf->tokens[ tok_num ].offset), is_closing_token );

  return BINSON_RES_OK;
}


/** \brief
 *
 * \param tbuf binson_token_buf*
 * \param tok_num uint8_t
 * \param pptr uint8_t**
 * \param psize binson_raw_size*
 * \return binson_res
 */
binson_res  binson_token_buf_get_token_payload( binson_token_buf *tbuf, uint8_t tok_num, binson_raw_value *raw_val )
{
  binson_res  res = BINSON_RES_OK;
  uint8_t     *sig_ptr, *payload_ptr;

  if (!tbuf || tok_num >= BINSON_TOKEN_BUF_TOKS || tok_num > tbuf->current_token ||!raw_val)
    return BINSON_RES_ERROR_ARG_WRONG;

  if (!tbuf->is_valid)
    return BINSON_RES_ERROR_PARSE_INVALID_INPUT;

  if (tbuf->tokens[ tok_num ].is_partial)
    return BINSON_RES_ERROR_PARSE_PART;

  sig_ptr = tbuf->ptr + tbuf->tokens[ tok_num ].offset;
  payload_ptr = sig_ptr + BINSON_RAW_SIG_SIZE + tbuf->tokens[ tok_num ].len_size;

  switch (*sig_ptr)
  {
    case BINSON_SIG_TRUE:
      raw_val->bool_val = true;
    break;

    case BINSON_SIG_FALSE:
      raw_val->bool_val = false;
    break;

    case BINSON_SIG_DOUBLE:
      raw_val->double_val = binson_util_unpack_double( payload_ptr );
    break;

    case BINSON_SIG_INTEGER_8:
      raw_val->int_val = binson_util_unpack_integer( payload_ptr, 1 );
    break;

    case BINSON_SIG_INTEGER_16:
      raw_val->int_val = binson_util_unpack_integer( payload_ptr, 2 );
    break;

    case BINSON_SIG_INTEGER_32:
      raw_val->int_val = binson_util_unpack_integer( payload_ptr, 4 );
    break;

    case BINSON_SIG_INTEGER_64:
      raw_val->int_val = binson_util_unpack_integer( payload_ptr, 8 );
    break;

    case BINSON_SIG_STRING_8:
    case BINSON_SIG_STRING_16:
    case BINSON_SIG_STRING_32:
    case BINSON_SIG_BYTES_8:
    case BINSON_SIG_BYTES_16:
    case BINSON_SIG_BYTES_32:
      raw_val->bbuf_val.bptr = payload_ptr;
      raw_val->bbuf_val.bsize = tbuf->tokens[ tok_num ].val_size;
    break;

    default:
      res = BINSON_RES_ERROR_PARSE_INVALID_INPUT;
    break;
  }

  return res;
}

/** \brief
 *
 * \param tbuf binson_token_buf*
 * \param tok_num uint8_t
 * \param psig uint8_t*
 * \return binson_res
 */
binson_res  binson_token_buf_get_sig( binson_token_buf *tbuf, uint8_t tok_num, uint8_t *psig )
{
  if (!tbuf || tok_num >= BINSON_TOKEN_BUF_TOKS || !psig)
    return BINSON_RES_ERROR_ARG_WRONG;

  *psig =  *(tbuf->ptr + tbuf->tokens[ tok_num ].offset);

  return BINSON_RES_OK;
}

/** \brief
 *
 * \param tbuf binson_token_buf*
 * \param pbool bool*
 * \return binson_res
 *
 */
binson_res  binson_token_buf_is_partial( binson_token_buf *tbuf, bool *pbool )
{
  *pbool = (tbuf->tokens_requested == tbuf->current_token && !tbuf->tokens[ tbuf->current_token ].is_partial)? false : true;
  return BINSON_RES_OK;
}

/** \brief
 *
 * \param tbuf binson_token_buf*
 * \param pbool bool*
 * \return binson_res
 *
 */
binson_res  binson_token_buf_is_valid( binson_token_buf *tbuf, bool *pbool )
{
  *pbool = tbuf->is_valid;
  return BINSON_RES_OK;
}

/** \brief
 *
 * \param tbuf binson_token_buf*
 * \param tok_num uint8_t
 * \param pptr uint8_t**
 * \return binson_res
 *
 */
binson_res  binson_token_buf_get_token_ptr( binson_token_buf *tbuf, uint8_t tok_num, uint8_t **pptr )
{
  if (!tbuf || tok_num >= BINSON_TOKEN_BUF_TOKS || !pptr)
    return BINSON_RES_ERROR_ARG_WRONG;

  *pptr = tbuf->ptr + tbuf->tokens[tok_num].offset;

  return BINSON_RES_OK;
}

/** \brief
 *
 * \param tbuf binson_token_buf*
 * \param tok_num uint8_t
 * \param pbsize binson_raw_size*
 * \return binson_res
 *
 */
binson_res  binson_token_buf_get_token_size( binson_token_buf *tbuf, uint8_t tok_num, binson_raw_size *pbsize )
{
  if (!tbuf || tok_num >= BINSON_TOKEN_BUF_TOKS || !pbsize)
    return BINSON_RES_ERROR_ARG_WRONG;

  *pbsize = tbuf->tokens[tok_num].size;

  return BINSON_RES_OK;
}
