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
 * \file binson_strstack.h
 * \brief String stack implementation
 *
 * \author Alexander Reshniuk
 * \date 20/11/2015
 *
 ***********************************************/

#include "binson_strstack.h"

typedef struct   _binson_strstack_t
{
  char*    buf_ptr;
  size_t   buf_size;
  size_t   buf_used;

} binson_strstack_t;



binson_strstack_init( binson_strstack_t *strbuf, char *ptr, size_t size )
{
  strbuf->buf_ptr = ptr;
  strbuf->buf_size = size;

  strbuf->buf_used = 0;

}

/********************************************//**
 * \brief
 *
 * \param
 * \param
 * \return
 *
 ***********************************************/
binson_strstack_free( binson_strstack_t *strbuf )
{
  /**< Since no mallocs in strstack implementation it's trivial */
  strbuf->buf_used = 0;
}

binson_strstack_lease( binson_strstack_t *strbuf, char **ptr, size_t size )
{
    *ptr = strbuf->buf_ptr + strbuf->buf_used;
    *(ptr+size) = size;

    strbuf->buf_used += size + sizeof(size_t);
}

binson_strstack_release( binson_strstack_t *strbuf, char *ptr )
{
  size_t last_size =  *(ptr + strbuf->buf_used - sizeof(size_t));
  char*  last_ptr =  ptr + strbuf->buf_used - sizeof(size_t) - last_size;

  if (ptr != last_ptr)
    return errr; //panic

  strbuf->buf_used -= (last_size + sizeof(size_t));

}

binson_strstack_getfree( binson_strstack_t *strbuf )
{
  return  strbuf->buf_size - strbuf->buf_used;
}
