/*
 *  Copyright (c) 2015 ASSA ABLOY AB
 *
 *  This file is part of binson-c, BINSON serialization format library in C.
 *
 *  SPDX-License-Identifier: MIT
 *  See the LICENSE file in the project root for the full license text.
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
#include "binson_common.h"
#include "binson_error.h"
#include "binson_io.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
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

/*
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