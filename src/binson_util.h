/*
 *  Copyright (c) 2015 ASSA ABLOY AB
 *
 *  This file is part of binson-c, BINSON serialization format library in C.
 *
 *  SPDX-License-Identifier: MIT
 *  See the LICENSE file in the project root for the full license text.
 */

/********************************************//**
 * \file binson_util.h
 * \brief Utility functions header file
 *
 * \author Alexander Reshniuk
 * \date 20/11/2015
 *
 ***********************************************/

#ifndef BINSON_UTIL_H_INCLUDED
#define BINSON_UTIL_H_INCLUDED

#include <stddef.h>

#include "binson_config.h"
#include "binson/binson_error.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  Useful macros missing in C89
 */
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

/*
 *  Conversion helpers (binson raw <-> C style)
 */
size_t	binson_util_pack_integer( int64_t val, uint8_t *bbuf );
size_t	binson_util_pack_double( double val, uint8_t *bbuf );

int64_t	binson_util_unpack_integer( const uint8_t *bbuf, uint8_t bsize );
double	binson_util_unpack_double( const uint8_t *bbuf );

#ifdef __cplusplus
}
#endif

#endif /* BINSON_UTIL_H_INCLUDED */