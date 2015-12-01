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
 * \file binson_util.c
 * \brief Utility functions implementation
 *
 * \author Alexander Reshniuk
 * \date 20/11/2015
 *
 ***********************************************/

#include "binson_util.h"


uint16_t binson_util_uint16_swap( uint16_t i )
{
    return (i << 8) | (i >> 8 );
}


int16_t binson_util_int16_swap( int16_t i )
{
    return (i << 8) | ((i >> 8) & 0xff);
}

uint32_t binson_util_uint32_swap( uint32_t i )
{
    i = ((i << 8) & 0xff00ff00 ) | ((i >> 8) & 0xff00ff );
    return (i << 16) | (i >> 16);
}

int32_t binson_util_int32_swap( int32_t i )
{
    i = ((i << 8) & 0xff00ff00) | ((i >> 8) & 0xff00ff );
    return (i << 16) | ((i >> 16) & 0xffff);
}

int64_t binson_util_int64_swap( int64_t i )
{
    i = ((i << 8) & 0xff00ff00ff00ff00ull ) | ((i >> 8) & 0x00ff00ff00ff00ffull );
    i = ((i << 16) & 0xffff0000ffff0000ull ) | ((i >> 16) & 0x0000ffff0000ffffull );
    return (i << 32) | ((i >> 32) & 0xffffffffull);
}

uint64_t binson_util_uint64_swap( uint64_t i )
{
    i = ((i << 8) & 0xff00ff00ff00ff00ull ) | ((i >> 8) & 0x00ff00ff00ff00ffull );
    i = ((i << 16) & 0xffff0000ffff0000ull ) | ((i >> 16) & 0x0000ffff0000ffffull );
    return (i << 32) | (i >> 32);
}

uint8_t binson_util_get_significant_bytes( int64_t i )
{
  int cnt, idx=0;

  i = (i<0)? -i:i;   // remove sign

  for (cnt=1; cnt<=9; cnt++)
  {
    if (i & 0xff)
      idx  = cnt;
    i >>= 8;
  }

  if (!idx)  // zero value still needs one byte to be stored
    idx = 1;

  return idx;
}

/*
bool is_LE_arch() {  // detect LE/BE arch
    int i = 1;
    char *p = (char *)&i;

    return (p[0] == 1)? true:false;
}*/

/** \brief Convert 64-bit arg to LE representation in memory buffer
 *
 * \param val int64_t                 Value
 * \param bbuf uint8_t*               Destination byte buffer
 * \param expand_to_next_int bool     Expand number of bytes to next int size (e.g. 3->4, 5->8 bytes)
 * \return size_t                     Result width in bytes
 */
size_t binson_util_pack_integer( int64_t val, uint8_t *bbuf, bool expand_to_next_int )
{
  const uint8_t int_map[] = { 1, 1, 2, 4, 4, 8, 8, 8, 8 }; /**< Maps number of bytes to closes int size */

  size_t size, empty_cnt = 0;
  bool neg = (val<0);

  for (int i=0; i<sizeof(int64_t); i++)
  {
     bbuf[i] = val & 0xff;
     empty_cnt = (bbuf[i] == (neg? 0xff:0x00))? empty_cnt+1 : 0;
     val >>= 8;
  }

  size = sizeof(int64_t)-empty_cnt;

  return expand_to_next_int? int_map[size] : (size? size:1);   /**< Zero value still requires one byte of storage */
}

/** \brief Convert 64-bit \c double to LE representation in memory buffer
 *
 * \param val double      Value
 * \param bbuf uint8_t*   Destination byte buffer
 * \return size_t         Result width in bytes
 */
size_t binson_util_pack_double( double val, uint8_t *bbuf )
{
  binson_util_pack_integer(  (int64_t)val, bbuf, false );
  return sizeof(double);
}

