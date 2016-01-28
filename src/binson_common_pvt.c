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
 * \file binson_common_pvt.c
 * \brief Common private implementation details
 *
 * \author Alexander Reshniuk
 * \date 08/12/2015
 *
 ***********************************************/

#include "binson_common_pvt.h"

/* \brief
 *
 * \param sig enum
 * \param pclosing_tag bool*  true, if sig is closing part of OBJECT/ARRAY
 * \return binson_node_type
 */
binson_node_type   binson_common_map_sig_to_node_type( uint8_t sig, bool *pclosing_tag )
{
  if (pclosing_tag)
  {
    if (sig == BINSON_SIG_OBJ_END || sig == BINSON_SIG_ARRAY_END)
      *pclosing_tag = true;
    else
      *pclosing_tag = false;
  }

  switch (sig)
  {
    case BINSON_SIG_OBJ_BEGIN:
    case BINSON_SIG_OBJ_END:
      return BINSON_TYPE_OBJECT;

    case BINSON_SIG_ARRAY_BEGIN:
    case BINSON_SIG_ARRAY_END:
      return BINSON_TYPE_ARRAY;

    case BINSON_SIG_TRUE:
    case BINSON_SIG_FALSE:
      return BINSON_TYPE_BOOLEAN;

    case BINSON_SIG_DOUBLE:
      return BINSON_TYPE_DOUBLE;

    case BINSON_SIG_INTEGER_8:
    case BINSON_SIG_INTEGER_16:
    case BINSON_SIG_INTEGER_32:
    case BINSON_SIG_INTEGER_64:
      return BINSON_TYPE_INTEGER;

    case BINSON_SIG_STRING_8:
    case BINSON_SIG_STRING_16:
    case BINSON_SIG_STRING_32:
      return BINSON_TYPE_STRING;

    case BINSON_SIG_BYTES_8:
    case BINSON_SIG_BYTES_16:
    case BINSON_SIG_BYTES_32:
      return BINSON_TYPE_BYTES;

    default:
    return BINSON_TYPE_UNKNOWN;
  }
}