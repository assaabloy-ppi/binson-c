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

/** \brief
 *
 * \param i int64_t
 * \return uint8_t
 */
uint8_t binson_util_get_significant_bytes( int64_t i )
{
  int cnt, idx=0;

  i = (i<0)? -i:i;   /* remove sign */

  for (cnt=1; cnt<=9; cnt++)
  {
    if (i & 0xff)
      idx  = cnt;
    i >>= 8;
  }

  if (!idx)  /* zero value still needs one byte to be stored */
    idx = 1;

  return idx;
}

/** \brief Convert 64-bit arg to LE representation in memory buffer
 *
 * \param val int64_t                 Value
 * \param bbuf uint8_t*               Destination byte buffer
 * \param expand_to_next_int bool     Expand number of bytes to next int size (e.g. 3->4, 5->8 bytes)
 * \return size_t                     Result width in bytes
 */
size_t binson_util_pack_integer( int64_t val, uint8_t *bbuf, bool expand_to_next_int )
{
  size_t i;
  const uint8_t int_map[] = { 1, 1, 2, 4, 4, 8, 8, 8, 8 }; /**< Maps number of bytes to closes int size */

  size_t size, empty_cnt = 0;
  bool neg = (val<0);

  for (i=0; i<sizeof(int64_t); i++)
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
  union {
    double dval;
    int64_t ival;
  } utmp;

  utmp.dval = val;

  binson_util_pack_integer( utmp.ival, bbuf, false );
  return sizeof(double);
}

