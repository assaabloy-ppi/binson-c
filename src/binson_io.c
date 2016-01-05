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
 * \file binson_io.h
 * \brief Binson input/output abstraction layer implementation
 *
 * \author Alexander Reshniuk
 * \date 20/11/2015
 *
 ***********************************************/

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

#include "binson/binson_io.h"
#include "binson_util.h"

/**< String buffer access struct */
typedef struct _binson_io_strbuf_struct {

  char      *ptr;
  size_t     buf_size;
  size_t     cursor;

} binson_io_strbuf_struct;

/**< Memory byte buffer access struct */
typedef struct _binson_io_bytebuf_struct {

  uint8_t   *ptr;
  size_t     buf_size;
  size_t     cursor;

} binson_io_bytebuf_struct;

/**< Union of supported storage structs */
typedef union _binson_io_struct {

  binson_io_strbuf_struct       strbuf;
  binson_io_bytebuf_struct      bytebuf;
  FILE                          *stream;

} binson_io_struct;

/**< Binson IO context */
typedef struct binson_io_ {

  binson_io_type    type;
  binson_io_mode    mode;
  binson_io_struct  handle;

  binson_res          status;        /**< Last input/output operation result */
  int                 errno_copy;    /**< Last errno value for this object */

} binson_io_;

/** \brief Allocate new \c binson_io context object
 *
 * \param io binson_io*
 * \return binson_res
 *
 */
binson_res  binson_io_new( binson_io **pio )
{
  *pio = (binson_io *)malloc(sizeof(binson_io_));
  return BINSON_RES_OK;
}

/** \brief
 *
 * \param io binson_io*
 * \return binson_res
 */
binson_res  binson_io_init( binson_io *io )
{
  io->type = BINSON_IO_TYPE_NULL;
  io->mode = BINSON_IO_MODE_NONE;
  io->handle.stream = 0;
  io->status = BINSON_RES_OK;
  io->errno_copy = 0;

  return BINSON_RES_OK;
}

/** \brief Free \c binson_io context object
 *
 * \param io binson_io*
 * \return binson_res
 *
 */
binson_res  binson_io_free( binson_io *io )
{
  binson_res  res;

  if (!io)
    return BINSON_RES_ERROR_ARG_WRONG;

  res = binson_io_close( io );
  free( io );

  return res;
}

/** \brief Open file with specified access mode and attach it to \c binson_io object
 *
 * \param obj binson_io*          Context
 * \param path const char*        File path
 * \param mode binson_io_mode_t   Access mode
 * \return binson_res             Result code
 */
binson_res  binson_io_open_file( binson_io *obj, const char* path, binson_io_mode mode )
{
  char m[3] = {0};

  if (!obj || !path)
    return BINSON_RES_ERROR_ARG_WRONG;

  obj->type = BINSON_IO_TYPE_STREAM;
  obj->mode = mode;

  if (mode & BINSON_IO_MODE_READ)
    strcat(m, "r");
  else
    if (mode & BINSON_IO_MODE_APPEND)
      strcat(m, "a");
    else
      strcat(m, "w");

  return binson_io_attach_stream( obj, fopen(path, m) );
}

/** \brief Attach already opened \c FILE stream to \c binson_io object
 *
 * \param obj binson_io*  Context
 * \param stream FILE*    Stream handle
 * \return binson_res     Result code
 */
binson_res  binson_io_attach_stream( binson_io *obj, FILE *stream )
{
  if (!obj || !stream)
    return BINSON_RES_ERROR_ARG_WRONG;

  obj->type = BINSON_IO_TYPE_STREAM;

  obj->handle.stream = stream;
  obj->status = (!obj->handle.stream)? BINSON_RES_OK :BINSON_RES_ERROR_STREAM;
  obj->errno_copy = errno;

  return BINSON_RES_OK;
}

/** \brief Attach already existing file descriptor to \c binson_io object
 *
 * \param obj binson_io*          Context
 * \param fd int                  File descriptor
 * \param mode binson_io_mode_t   Access mode
 * \return binson_res             Result code
 *
 */
/*binson_res  binson_io_attach_fd( binson_io *obj, int fd, binson_io_mode mode  )
{
  char m[3] = {0};

  if (!obj)
    return BINSON_RES_ERROR_ARG_WRONG;

  obj->mode = mode;

  if (mode & BINSON_IO_MODE_READ)
    strcat(m, "r");
  else
    if (mode & BINSON_IO_MODE_APPEND)
      strcat(m, "a");
    else
      strcat(m, "w");

  return binson_io_attach_stream( obj, fdopen(fd, &m) );
}*/

/** \brief Attach string buffer to \c binson_io object
 *
 * \param obj binson_io*          Context
 * \param str char*               String buffer pointer
 * \param str_size size_t         String buffer size
 * \param mode binson_io_mode_t   Access mode
 * \return binson_res             Result code
 */
binson_res  binson_io_attach_str( binson_io *obj, char* str, size_t str_size, binson_io_mode mode )
{
  if (!obj || !str || !str_size)
    return BINSON_RES_ERROR_ARG_WRONG;

  obj->type = BINSON_IO_TYPE_STR0;
  obj->mode = mode;
  obj->handle.strbuf.ptr = str;
  obj->handle.strbuf.buf_size = str_size;
  obj->handle.strbuf.cursor = (mode & BINSON_IO_MODE_APPEND)? strlen(str) : 0;
  obj->status = BINSON_RES_OK;

  return BINSON_RES_OK;
}

/** \brief Attach byte buffer to \c binson_io object
 *
 * \param obj binson_io*    Context
 * \param buf uint8_t*      Byte buffer pointer
 * \param buf_size size_t   Byte buffer size
 * \return binson_res       Result code
 */
binson_res  binson_io_attach_bytebuf( binson_io *obj, uint8_t *buf, size_t buf_size )
{
  if (!obj || !buf || !buf_size)
    return BINSON_RES_ERROR_ARG_WRONG;

  obj->type = BINSON_IO_TYPE_BUFFER;
  obj->handle.bytebuf.ptr = buf;
  obj->handle.bytebuf.buf_size = buf_size;
  obj->handle.strbuf.cursor = 0;
  obj->status = BINSON_RES_OK;

  return BINSON_RES_OK;
}

/** \brief Close \c binson_io object and free internal resources
 *
 * \param obj binson_io*    Context
 * \return binson_res       Result code
 */
binson_res  binson_io_close( binson_io *obj )
{
  int res = 0;

  if (!obj)
    return BINSON_RES_ERROR_ARG_WRONG;

  if (obj->type == BINSON_IO_TYPE_STREAM)
    res = fclose(obj->handle.stream);

  if (res)
  {
    obj->status = BINSON_RES_ERROR_STREAM;
    obj->errno_copy = errno;
  }
  else
    obj->status = BINSON_RES_OK;

  obj->type = BINSON_IO_TYPE_NULL;
  obj->handle.stream = NULL;

  return  obj->status;
}

/** \brief Writes data from external byte buffer to already opened \c binson_io
 *
 * \param obj binson_io*      Context
 * \param src_ptr uint8_t*    Byte buffer pointer
 * \param block_size size_t   Number of bytes to write
 * \return binson_res         Result code
 */
binson_res  binson_io_write( binson_io *obj, const uint8_t *src_ptr, size_t block_size )
{
  binson_res res = BINSON_RES_OK;
  size_t     written;

  if (!obj || !src_ptr)
    return BINSON_RES_ERROR_ARG_WRONG;

  if (!block_size)
    return res;

  switch (obj->type)
  {
    case BINSON_IO_TYPE_STREAM:
      written = fwrite(src_ptr, 1, block_size, obj->handle.stream);
      res = (block_size == written)? BINSON_RES_OK : BINSON_RES_ERROR_STREAM;
    break;

    case BINSON_IO_TYPE_STR0:
    case BINSON_IO_TYPE_BUFFER:
      if (obj->handle.bytebuf.cursor + block_size > obj->handle.bytebuf.buf_size)
        return BINSON_RES_ERROR_IO_OUT_OF_BUFFER;
      else
      {
        memcpy(obj->handle.bytebuf.ptr + obj->handle.bytebuf.cursor, src_ptr, block_size);
        obj->handle.bytebuf.cursor += block_size;
      }
      break;

    case BINSON_IO_TYPE_NULL:
    default:
    return BINSON_RES_ERROR_BROKEN_INT_STRUCT;
  }

  return res;
}

/** \brief Write zero-terminated string to opened \c binson_io
 *
 * \param obj binson_io*          Context
 * \param str const char*         Source string pointer
 * \param write_terminator bool   Zero terminator write flag
 * \return binson_res             Result code
 */
binson_res  binson_io_write_str( binson_io *obj, const char* str, bool write_terminator )
{
   return binson_io_write(obj, (const uint8_t *)str, strlen(str) + (write_terminator? 1:0));
}

/** \brief Write single byte to opened \c binson_io
 *
 * \param obj binson_io*  Context
 * \param byte uint8_t    Byte value to write
 * \return binson_res     Result code
 */
binson_res  binson_io_write_byte( binson_io *obj, uint8_t byte )
{
  return binson_io_write( obj, &byte, 1 );
}

/** \brief Writes printf() style formated output to opened \c binson_io.
 *         (\c va_list argument allows to make wrappers around this function)
 *
 * \param obj binson_io*        Context
 * \param format const char*    Format string. Format is same as used for \c printf()
 * \param args va_list          Variadic arguments
 * \return binson_res           Result code
 */
binson_res  binson_io_vprintf( binson_io *obj, const char* format, va_list args )
{
  unsigned int written = 0;

  if (!obj )
    return BINSON_RES_ERROR_ARG_WRONG;

  switch (obj->type)
  {
    case BINSON_IO_TYPE_STREAM:
      written = (unsigned int)vfprintf( obj->handle.stream, format, args );
    break;

    case BINSON_IO_TYPE_STR0:
    case BINSON_IO_TYPE_BUFFER:
        /**< \todo Implement strict overflow checks to mimic nonportable vsnprintf() */
       written = (unsigned int)vsprintf( (obj->mode & BINSON_IO_MODE_APPEND)? obj->handle.strbuf.ptr + obj->handle.strbuf.cursor :
                                                                obj->handle.strbuf.ptr, format, args );
       obj->handle.strbuf.cursor += written;
    break;

    case BINSON_IO_TYPE_NULL:
    break;

    default:
    return BINSON_RES_ERROR_ARG_WRONG;
  }

  return written? BINSON_RES_OK : BINSON_RES_ERROR_IO_TYPE_UNKNOWN;
}

/** \brief Writes printf() style formated output to opened \c binson_io.
 *
 * \param obj binson_io*        Context
 * \param format const char*    Format string. Format is same as used for \c printf()
 * \param ...                   Variable list of arguments
 * \return binson_res           Result code
 */
binson_res  binson_io_printf( binson_io *obj, const char* format, ... )
{
  binson_res res;
  va_list args;
  va_start( args, format );
  res = binson_io_vprintf( obj, format, args );
  va_end( args );

  return res;
}

/** \brief Read from \c binson_io up to \c max_size bytes to external buffer
 *
 * \param obj binson_io*      Context
 * \param dst_ptr uint8_t*    Destination byte buffer
 * \param max_size size_t     Buffer size limit
 * \param read_bytes size_t*  Number of bytes successfully read
 * \return binson_res         Result code
 */
binson_res  binson_io_read( binson_io *obj, uint8_t *dst_ptr, size_t max_size, size_t *read_bytes )
{
  binson_res  res = BINSON_RES_OK;
  size_t      cnt;

  if (!obj || !dst_ptr)
    return BINSON_RES_ERROR_ARG_WRONG;

  if (!max_size)
    return res;

  switch (obj->type)
  {
    case BINSON_IO_TYPE_STREAM:
      *read_bytes = fread(dst_ptr, 1, max_size, obj->handle.stream );
      res = (*read_bytes == max_size )? BINSON_RES_OK : BINSON_RES_ERROR_STREAM;
    break;

    case BINSON_IO_TYPE_STR0:
    case BINSON_IO_TYPE_BUFFER:
        /**< can't copy more than we have */
        cnt = MIN(max_size, obj->handle.bytebuf.buf_size - obj->handle.bytebuf.cursor );

        memcpy(dst_ptr, obj->handle.bytebuf.ptr + obj->handle.bytebuf.cursor, cnt);
        obj->handle.bytebuf.cursor += cnt;
	*read_bytes = cnt;
	res = (cnt < max_size)? BINSON_RES_ERROR_IO_OUT_OF_BUFFER : BINSON_RES_OK;
        break;

    case BINSON_IO_TYPE_NULL:
    default:
    return BINSON_RES_ERROR_BROKEN_INT_STRUCT;
  }

  return res;
}

/** \brief Read from \c binson_io up to max_size bytes to
 *         string buffer \c strbuf add trailing zero terminator
 *
 * \param obj binson_io*          Context
 * \param strbuf char*            Destination string buffer pointer
 * \param max_size size_t         String buffer size
 * \param read_chars size_t*      Number of chars successfully read
 * \param mode binson_io_mode_t   Specify BINSON_IO_MODE_APPEND to concatenate to existing string
 * \return binson_res             Result code
 */
binson_res  binson_io_read_str( binson_io *obj, char* strbuf, size_t max_size, size_t *read_chars, binson_io_mode mode  )
{
  binson_res res = BINSON_RES_OK;
  size_t len = 0;

  if (mode & BINSON_IO_MODE_APPEND)
  {
    len = strlen(strbuf);
    res = binson_io_read( obj, (uint8_t *)strbuf+len, max_size, read_chars );
  }
  else
    res = binson_io_read( obj, (uint8_t *)strbuf, max_size, read_chars );

  strbuf[len + *read_chars] = 0; /**< Terminating zero */

  return res;
}
