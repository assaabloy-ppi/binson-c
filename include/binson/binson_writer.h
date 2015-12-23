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
 * \file binson_writer.h
 * \brief Binson format writer API header file
 *
 * \author Alexander Reshniuk
 * \date 20/11/2015
 *
 ***********************************************/

#ifndef BINSON_WRITER_H_INCLUDED
#define BINSON_WRITER_H_INCLUDED

#include "binson_config.h"
#include "binson/binson_common.h"
#include "binson/binson_error.h"
#include "binson/binson_io.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  Forward declarations
 */
typedef struct binson_writer_  binson_writer;

/**
 *  Binson writer output formats enum type
 */
typedef enum {
  BINSON_WRITER_FORMAT_RAW  = 0,    /**< Raw binary Binson format (see Binson specs) */
  BINSON_WRITER_FORMAT_HEX,         /**< Hex string representation, e.g. "0x41 0x33 0x18 0x40" */
#ifdef WITH_BINSON_JSON_OUTPUT
  BINSON_WRITER_FORMAT_JSON,        /**< JSON text format without extra white spaces */
  BINSON_WRITER_FORMAT_JSON_NICE,   /**< JSON text format with white space indents */
#endif
  BINSON_WRITER_FORMAT_LAST         /**< Enum terminator. Need for arg validation */

} binson_writer_format;

/**
 *  Binson/JSON low-level output API calls
 */
binson_res  binson_writer_new( binson_writer **pwriter);
binson_res  binson_writer_init( binson_writer *writer, binson_io *io, binson_writer_format format  );
binson_res  binson_writer_free( binson_writer *writer );
binson_res  binson_writer_set_format( binson_writer *writer, binson_writer_format format );
binson_res  binson_writer_set_io( binson_writer *writer, binson_io *io );
binson_io*  binson_writer_get_io( binson_writer *writer );

binson_res  binson_writer_write_token( binson_writer *writer, binson_token_type token_type, const char* key, binson_value *val );

binson_res  binson_writer_write_object_begin( binson_writer *writer, const char* key );
binson_res  binson_writer_write_object_end( binson_writer *writer );
binson_res  binson_writer_write_array_begin( binson_writer *writer, const char* key );
binson_res  binson_writer_write_array_end( binson_writer *writer );
binson_res  binson_writer_write_boolean( binson_writer *writer, const char* key, bool val );
binson_res  binson_writer_write_integer( binson_writer *writer, const char* key, int64_t val );
binson_res  binson_writer_write_double( binson_writer *writer, const char* key, double val );
binson_res  binson_writer_write_str( binson_writer *writer, const char* key, const char* str );
binson_res  binson_writer_write_bytes( binson_writer *writer, const char* key, uint8_t *src_ptr,  size_t src_size );


#ifdef __cplusplus
}
#endif

#endif /* BINSON_WRITER_H_INCLUDED */
