/*
 *  Copyright (c) 2015 ASSA ABLOY AB
 *
 *  This file is part of binson-c, BINSON serialization format library in C.
 *
 *  SPDX-License-Identifier: MIT
 *  See the LICENSE file in the project root for the full license text.
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

} binson_raw_value;

#ifdef __cplusplus
}
#endif

#endif /* BINSON_COMMON_H_INCLUDED */