/*
 *  Copyright (c) 2015 ASSA ABLOY AB
 *
 *  This file is part of binson-c, BINSON serialization format library in C.
 *
 *  SPDX-License-Identifier: MIT
 *  See the LICENSE file in the project root for the full license text.
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