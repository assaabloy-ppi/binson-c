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
 * \file binson_common.h
 * \brief Data structures declaration common for all public interfaces
 *
 * \author Alexander Reshniuk
 * \date 08/12/2015
 *
 ***********************************************/

#ifndef BINSON_COMMON_H_INCLUDED
#define BINSON_COMMON_H_INCLUDED

#include "binson_config.h"
#include "binson_error.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  Supported node types
 */
typedef enum binson_node_type
{
  BINSON_TYPE_UNKNOWN    = 0,
  BINSON_TYPE_OBJECT,
  BINSON_TYPE_ARRAY,
  BINSON_TYPE_BOOLEAN,
  BINSON_TYPE_INTEGER,
  BINSON_TYPE_DOUBLE,
  BINSON_TYPE_STRING,
  BINSON_TYPE_BYTES

} binson_node_type;

/**
 *  Used by 'binson_writer' and 'binson_parser'
 */
typedef enum binson_token_type
{
  BINSON_TOKEN_TYPE_UNKNOWN       = 0,
  BINSON_TOKEN_TYPE_OBJECT_BEGIN,
  BINSON_TOKEN_TYPE_OBJECT_END,
  BINSON_TOKEN_TYPE_ARRAY_BEGIN,
  BINSON_TOKEN_TYPE_ARRAY_END,
  BINSON_TOKEN_TYPE_BOOLEAN,
  BINSON_TOKEN_TYPE_INTEGER,
  BINSON_TOKEN_TYPE_DOUBLE,
  BINSON_TOKEN_TYPE_STRING,
  BINSON_TOKEN_TYPE_BYTES,

  BINSON_TOKEN_TYPE_LAST

} binson_token_type;

/**
 *  Payload data type
 */
typedef union binson_value {

    bool      bool_val;
    int64_t   int_val;
    double    double_val;

    char      *str_val;

    struct bbuf_val
    {
      uint8_t         *bptr;
      binson_size      bsize;

    } bbuf_val;

} binson_value;


/**
 *  Raw payload data type. String are NOT zero terminated
 */
typedef union binson_raw_value {

    bool      bool_val;
    int64_t   int_val;
    double    double_val;

    struct bbuf_val bbuf_val;
  /*  {
      uint8_t         *bptr;
      binson_size      bsize;

    } bbuf_val;*/

} binson_raw_value;


#ifdef __cplusplus
}
#endif

#endif /* BINSON_COMMON_H_INCLUDED */
