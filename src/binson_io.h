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
 * \file binson_io.h
 * \brief Binson input/output abstraction layer header file
 *
 * \author Alexander Reshniuk
 * \date 20/11/2015
 *
 ***********************************************/

#ifndef BINSON_IO_H_INCLUDED
#define BINSON_IO_H_INCLUDED

#include <stdio.h>
#include <stdarg.h>

#include "binson_config.h"
#include "binson_error.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "binson_config.h"
#include "binson_error.h"

/**
 *  Forward declarations
 */
#ifndef binson_io_DEFINED
typedef struct binson_io_  binson_io;
# define binson_io_DEFINED
#endif

/**
 *  Supported data source/sink types
 */
typedef enum {
  BINSON_IO_TYPE_NULL = 0,      /**< Think it /dev/null */
  BINSON_IO_TYPE_STR0,          /**< Zero-terminated string */
  BINSON_IO_TYPE_BUFFER,        /**< Memory buffer */
  BINSON_IO_TYPE_STREAM         /**< Stdio stream (FILE) */

} binson_io_type;

/**
 *  Data access mode
 */
typedef enum {
  BINSON_IO_MODE_READ = 1,
  BINSON_IO_MODE_WRITE = 2,
  BINSON_IO_MODE_APPEND = 8,

  BINSON_IO_MODE_CREATE = 16

} binson_io_mode;

/**
 *  Binson IO abstraction layer API calls
 */
binson_res  binson_io_new( binson_io **pio );
binson_res  binson_io_free( binson_io *io );

binson_res  binson_io_open_file( binson_io *obj, const char* path, binson_io_mode mode );
binson_res  binson_io_attach_stream( binson_io *obj, FILE *stream );
/*binson_res  binson_io_attach_fd( binson_io *obj, int fd, binson_io_mode mode  );*/
binson_res  binson_io_attach_str( binson_io *obj, char* str, size_t str_size, binson_io_mode mode );
binson_res  binson_io_attach_bytebuf( binson_io *obj, uint8_t *buf, size_t buf_size);
binson_res  binson_io_close( binson_io *obj );

binson_res  binson_io_write( binson_io *obj, uint8_t *src_ptr, size_t block_size );
binson_res  binson_io_write_str( binson_io *obj, const char* str, bool write_terminator );
binson_res  binson_io_write_byte( binson_io *obj, uint8_t byte );

binson_res  binson_io_vprintf( binson_io *obj, const char* format, va_list args );
binson_res  binson_io_printf( binson_io *obj, const char* format, ... );

binson_res  binson_io_read( binson_io *obj, uint8_t *dst_ptr, size_t max_size, size_t *read_bytes );
binson_res  binson_io_read_str( binson_io *obj, char* strbuf, size_t max_size, size_t *read_chars, binson_io_mode mode );

#ifdef __cplusplus
}
#endif

#endif /* BINSON_IO_H_INCLUDED */
