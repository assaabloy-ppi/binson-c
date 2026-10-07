/*
 *  Copyright (c) 2015 ASSA ABLOY AB
 *
 *  This file is part of binson-c, BINSON serialization format library in C.
 *
 *  SPDX-License-Identifier: MIT
 *  See the LICENSE file in the project root for the full license text.
 */

/********************************************//**
 * \file binson_common_pvt.h
 * \brief Common private declarations
 *
 * \author Alexander Reshniuk
 * \date 08/12/2015
 *
 ***********************************************/

#ifndef BINSON_COMMON_PVT_H_INCLUDED
#define BINSON_COMMON_PVT_H_INCLUDED

#include "binson/binson_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Binson binary format 8-bit signatures */
#define BINSON_SIG_OBJ_BEGIN      0x40
#define BINSON_SIG_OBJ_END        0x41
#define BINSON_SIG_ARRAY_BEGIN    0x42
#define BINSON_SIG_ARRAY_END      0x43

#define BINSON_SIG_TRUE           0x44
#define BINSON_SIG_FALSE          0x45
#define BINSON_SIG_DOUBLE         0x46

#define BINSON_SIG_INTEGER_8      0x10
#define BINSON_SIG_INTEGER_16     0x11
#define BINSON_SIG_INTEGER_32     0x12
#define BINSON_SIG_INTEGER_64     0x13

#define BINSON_SIG_STRING_8       0x14
#define BINSON_SIG_STRING_16      0x15
#define BINSON_SIG_STRING_32      0x16

#define BINSON_SIG_BYTES_8        0x18
#define BINSON_SIG_BYTES_16       0x19
#define BINSON_SIG_BYTES_32       0x1a

binson_node_type  binson_common_map_sig_to_node_type( uint8_t sig, bool *pclosing_tag );

#ifdef __cplusplus
}
#endif

#endif /* BINSON_COMMON_PVT_H_INCLUDED */