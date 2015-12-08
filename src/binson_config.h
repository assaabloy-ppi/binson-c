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
 * \file binson_config.h
 * \brief binson-c library build configuration file
 *
 * \author Alexander Reshniuk
 * \date 20/11/2015
 *
 ***********************************************/

#ifndef BINSON_CONFIG_H_INCLUDED
#define BINSON_CONFIG_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  BEGIN of backport section
 */

/**< \c bool type backport. C89 have no \c bool type */
#ifndef bool
# define bool        int
# define true        1
# define false       0
#endif

/**< In C89 we have no strdint.h, so need to mimic it */
#ifndef __WORDSIZE
#if defined __x86_64__ && !defined __ILP32__
# define __WORDSIZE	64
#else
# define __WORDSIZE	32
#endif
#endif

/**
 *  Backport of fixed width types from C99's strdin.h
 */
typedef signed char		int8_t;
typedef short int		  int16_t;
typedef int			      int32_t;
# if __WORDSIZE == 64
typedef long int		  int64_t;
# else
typedef long long int	int64_t;
#endif

typedef unsigned char		        uint8_t;
typedef unsigned short int	    uint16_t;
typedef unsigned int		        uint32_t;
#if __WORDSIZE == 64
typedef unsigned long int	      uint64_t;
#else
typedef unsigned long long int	uint64_t;
#endif

/**
 *  Backport (from C99's \c strdin.h) of range defines
 */
# define INT8_MAX		  (127)
# define INT16_MAX		(32767)
# define INT32_MAX		(2147483647)
# define INT64_MAX    0x7fffffffffffffffLL

# define INT8_MIN		  (-128)
# define INT16_MIN		(-32767-1)
# define INT32_MIN		(-2147483647-1)
# define INT64_MIN    (-INT64_MAX - 1LL)

# define UINT8_MAX		(255)
# define UINT16_MAX		(65535)
# define UINT32_MAX		(4294967295U)

/* Limit of `size_t' type.  */
# if __WORDSIZE == 64
#  define SIZE_MAX		(18446744073709551615UL)
# else
#  define SIZE_MAX		(4294967295U)
# endif

/**
 *  END of backport section
 */

/**
 *  Detect endianness at build time
 */
#define IS_LITTLE_ENDIAN  (((*(short *)"xy") & 0xFF) == 'x')

/**< comment this for release lib builds */
#define DEBUG

/**< Uncoment this to disable \c assert() and \c BINSON_ASSERT() support */
/* #define NDEBUG */

#define ERROR_RING_SIZE  16  /**< Size of circular error buffer */

/* #define BINSON_MAX_CHILDREN   100 */
#define BINSON_CHILD_NUM_T      uint8_t
#define BINSON_NODE_NUM_T       uint16_t

typedef  uint32_t            binson_raw_offset;
typedef  uint32_t            binson_raw_size;
typedef  uint32_t            binson_size;
typedef  uint8_t             binson_depth;


typedef  BINSON_CHILD_NUM_T  binson_child_num;
typedef  BINSON_NODE_NUM_T   binson_node_num;

#define BINSON_DEPTH_LIMIT       16

#define  WITH_BINSON_PARSER_MODE_RAW           /**< Build with 'raw' model functionality */
#define  WITH_BINSON_PARSER_MODE_SMART         /**< Build with 'smart' model functionality */
#define  WITH_BINSON_PARSER_MODE_DOM           /**< Build with 'DOM' model functionality */

#define WITH_BINSON_JSON_OUTPUT                /**< Build \c binson_writer with JSON output support */
#define BINSON_JSON_OBJ_LENGTH_LIMIT     256   /**< Max number or chars in JSON-dumped representation of object */

#ifdef __cplusplus
}
#endif

#endif /* BINSON_CONFIG_H_INCLUDED */
