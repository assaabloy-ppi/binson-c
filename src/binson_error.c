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
 * \file binson_error.h
 * \brief Binson error handling implementation
 *
 * \author Alexander Reshniuk
 * \date 20/11/2015
 *
 ***********************************************/

#include <string.h>

#include "binson/binson_error.h"
#include "binson/binson_io.h"
#include "binson_util.h"

/**
 *  Binson error rec flags (possible values of flag member)
 */
typedef enum binson_error_rec_flag_ {

    BINSON_ERROR_ATOM_FLAG_MALLOCED,
    BINSON_ERROR_ATOM_FLAG_IS_TEXT

} binson_error_rec_flag;

/**
 *  Single error record structure
 */
typedef struct binson_error_rec_ {

    uint16_t                code;
    char*                   code_str;  /**< Optional. If not NULL, point to detailed code description */

    unsigned int            line;
    const char*             file;
    char*                   data;
    size_t                  data_len;

    binson_error_rec_flag  flag;

} binson_error_rec;

/**
 *  Error handling context
 */
typedef struct _binson_error {

  binson_error_rec     ring[ERROR_RING_SIZE];   /**< circular queue buffer */
  uint8_t              head, tail;
  binson_io*           io;

} binson_error;

/**
 *  Dummy guard union which must produce compiler warnings/errors in case of incorrect porting
 */
static union dummy_
{
    char   int8_t_incorrect[sizeof(  int8_t) == 1];
    char  uint8_t_incorrect[sizeof( uint8_t) == 1];
    char  int16_t_incorrect[sizeof( int16_t) == 2];
    char uint16_t_incorrect[sizeof(uint16_t) == 2];
    char  int32_t_incorrect[sizeof( int32_t) == 4];
    char uint32_t_incorrect[sizeof(uint32_t) == 4];
    char  int64_t_incorrect[sizeof( int64_t) == 8];
    char uint64_t_incorrect[sizeof(uint64_t) == 8];
} dummy_;

/** \brief Private helper. Get global/static instance of \c binson_error*
 *
 * \return binson_error*
 *
 */
binson_error*  get_context()
{
  static binson_error ERR = { {{0}}, 0, 0, NULL };    /**< The only instance per process exists ! */
  return &ERR;
}

/** \brief Init error handling context
 *
 * \param io binson_io*
 * \return binson_res
 */
binson_res  binson_error_init( binson_io *io )
{
  binson_error* err = get_context();

  UNUSED(dummy_);

  memset( err, 0, sizeof(binson_error) );
  err->io = io;

  return BINSON_RES_OK;
}

/** \brief Store error details in circular error queue
 *
 * \param res binson_res
 * \param file const char*
 * \param line unsigned int
 * \param data char*
 * \param data_len size_t
 * \return binson_res
 */
binson_res  binson_error_report( binson_res res, const char* file, unsigned int line, char *data, size_t data_len )
{
  binson_error* err = get_context();

  /**< Find next head position in ring */
  err->head = (err->head >= ERROR_RING_SIZE)? 0 : err->head+1;

  /**< Store error details */
  err->ring[err->head].code       = res;
  err->ring[err->head].code_str   = NULL;
  err->ring[err->head].line       = line;
  err->ring[err->head].file       = file;
  err->ring[err->head].data       = data;
  err->ring[err->head].data_len   = data_len;
  err->ring[err->head].flag       = 0;

  /**< Fix tail position if ring is full */
  if (err->head == err->tail)
    err->tail = (err->tail >= ERROR_RING_SIZE)? 0 : err->tail+1;

  return BINSON_RES_OK;
}

/** \brief Dump all stored errors to attached IO
 *
 * \return binson_res   Result code
 */
binson_res binson_error_dump()
{
  binson_error*      err = get_context();
  binson_res         res = BINSON_RES_OK;

  while (err->head != err->tail)
  {
    binson_error_rec*  rec;

    rec = &err->ring[err->head];   /**< Newest error dump first */
    res = binson_io_printf( err->io, "res=0x%04x, l=%d, f=%s, d=\"%s\"\n", rec->code, rec->line, rec->file, rec->data );
    err->head = (err->head == 0)? ERROR_RING_SIZE-1 : err->head-1;  /**< Roll back dumped error record */
  }

  return res;
}

/** \brief Clear all errors from ring buffer
 *
 * \return binson_res
 */
binson_res binson_error_clear_all()
{
    binson_error*  err = get_context();
    err->head = err->tail = 0;

    return BINSON_RES_OK;
}
