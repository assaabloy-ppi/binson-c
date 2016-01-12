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
 * \file binson_utf8.h
 * \brief UTF-8 utility functions header file
 *
 * \author Alexander Reshniuk
 * \date 20/11/2015
 *
 ***********************************************/
#ifndef BINSON_UTF8_H_INCLUDED
#define BINSON_UTF8_H_INCLUDED

#include "binson_config.h"
#include "binson/binson_error.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  UTF-8 string helpers
 */
bool   binson_utf8_is_valid( uint8_t* string );
size_t binson_utf8_unescape(uint8_t *buf, size_t sz, uint8_t *src);

/*size_t      binson_utf8_strlen_bytes( const char* str );
size_t      binson_utf8_strlen_chars( const char* str );
bool        binson_utf8_is_equal( const char* str1,  const char* str2 );
bool        binson_utf8_is_less( const char* str_left, const char* str_right );

binson_res  binson_utf8_encode( char* dst, const char* str );
binson_res  binson_utf8_decode( char* dst, const char* str );
binson_res  binson_utf8_validate( const char* str );
*/

#ifdef __cplusplus
}
#endif

#endif /* BINSON_UTF8_H_INCLUDED */
