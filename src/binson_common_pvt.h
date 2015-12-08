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
 * \file binson_common_pvt.h
 * \brief Inplementation specific details common for some modules
 *
 * \author Alexander Reshniuk
 * \date 08/12/2015
 *
 ***********************************************/

#ifndef BINSON_COMMON_PVT_H_INCLUDED
#define BINSON_COMMON_PVT_H_INCLUDED

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


/*binson_node_type   binson_common_map_sig_to_node_type( uint8_t sig );*/


#ifdef __cplusplus
}
#endif

#endif /* BINSON_COMMON_PVT_H_INCLUDED */
