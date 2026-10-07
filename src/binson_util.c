/*
 *  Copyright (c) 2015 ASSA ABLOY AB
 *
 *  This file is part of binson-c, BINSON serialization format library in C.
 *
 *  SPDX-License-Identifier: MIT
 *  See the LICENSE file in the project root for the full license text.
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

#define TWO_TO_7	128
#define TWO_TO_15	32768
#define	TWO_TO_31	2147483648L 

/** \brief Convert 64-bit arg to LE representation in memory buffer
 *
 * \param val int64_t                 Value
 * \param bbuf uint8_t*               Destination byte buffer
 * \return size_t                     Result width in bytes
 */
size_t binson_util_pack_integer( int64_t val, uint8_t *bbuf)
{
  size_t	i, size;
  
  if (val >= -TWO_TO_7 && val < TWO_TO_7) {
      size = 1;
  } else if (val >= -TWO_TO_15 && val < TWO_TO_15) {
      size = 2;
  } else if (val >= -TWO_TO_31 && val < TWO_TO_31) {
      size = 4;    
  } else {
      size = 8;       
  }  

  for (i=0; i<size; i++)
  {
     bbuf[i] = val & 0xff;
     val >>= 8;
  } 
  
  return size;
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
    double   dval;
    uint64_t uval;
  } utmp;
  size_t i;

  utmp.dval = val;

  /* always write all 8 bytes; shortest-form packing applies to integers only */
  for (i=0; i<sizeof(double); i++)
  {
    bbuf[i] = utmp.uval & 0xff;
    utmp.uval >>= 8;
  }
  return sizeof(double);
}


/** \brief Convert 64-bit integer LE representation  in byte buffer to 64-bit integer value
 *
 * \param bbuf uint8_t*
 * \param bsize uint8_t
 * \return int64_t
 */
int64_t  binson_util_unpack_integer( const uint8_t *bbuf, uint8_t bsize )
{
  int		i;
  int64_t	i64;

  i64 = bbuf[bsize-1] & 0x80 ? -1:0;  /* prefill with ones or zeroes depending of sign presence */

  for (i=bsize-1; i>=0; i--)
  {
    i64 <<= 8;
    i64 |= bbuf[i];
  }

  return i64;
}


/** \brief Convert 64-bit double LE representation in byte buffer to double value
 *
 * \param bbuf uint8_t*
 * \return double
 */
double  binson_util_unpack_double( const uint8_t *bbuf )
{
  union {
    double dval;
    int64_t ival;
  } utmp;

  utmp.ival = binson_util_unpack_integer( bbuf, sizeof(double) );
  return utmp.dval;
}