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
 * \file binson_error.h
 * \brief Binson error handling header file
 *
 * \author Alexander Reshniuk
 * \date 20/11/2015
 *
 ***********************************************/

#ifndef BINSON_ERROR_H_INCLUDED
#define BINSON_ERROR_H_INCLUDED

#include "binson_config.h"
#include "binson_io.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  Binson library error codes
 */
typedef enum binson_res_ {
    BINSON_RES_OK                       = 0,

    /**< traversal control codes set by callbacks */
    BINSON_RES_TRAVERSAL_BREAK          = 32,
    BINSON_RES_TRAVERSAL_RESTART,
    BINSON_RES_TRAVERSAL_DONE,
    BINSON_RES_TRAVERSAL_CB,    /**< problem in traversal's callback */

    /**< binson API calls argument errors */
    BINSON_RES_ERROR_ARG_WRONG,
    BINSON_RES_ERROR_ARG_WRONG_COMB,  /**< invalid argument combination */

    /**< tree access errors */
    BINSON_RES_ERROR_TREE_OUT_OF_ARRAY  = 128,    /**< unable to access ARRAY item specified */

    /**< binson raw data input errors */
    BINSON_RES_ERROR_IO_EOF             = 256,
    BINSON_RES_ERROR_IO_OUT_OF_BUFFER,
    BINSON_RES_ERROR_IO_PART,              /**< partial content */
    BINSON_RES_ERROR_IO_TYPE_UNKNOWN,

    /**< binson parsing errors */
    BINSON_RES_ERROR_PARSE_INVALID_SIG,
    BINSON_RES_ERROR_PARSE_SUSPENDED,       /**< Used from callback to postpone parsing */
    BINSON_RES_ERROR_PARSE_INVALID_STR     /**< String is not vilid UTF-8 string */

    /**< internal library errors/failures codes */
    BINSON_RES_ERROR_ASSERT_FAILED      = 512,
    BINSON_RES_ERROR_LIB_VERSION,         /**< binary lib version don't match headers version */
    BINSON_RES_ERROR_NOT_SUPPORTED,       /**< feature not supported in this binson model type or still not implemented in library */
    BINSON_RES_ERROR_OUT_OF_MEMORY,
    BINSON_RES_ERROR_BROKEN_INT_STRUCT,   /**< internal structure consistency is broken */
    BINSON_RES_ERROR_STREAM               /**<  stream/file access or read/write error */

} binson_res;

/**
 *  Binson error handling API calls
 */
binson_res     binson_error_init( binson_io *io );
binson_res     binson_error_report( binson_res res, const char* file, unsigned int line, char *data, size_t data_len );
binson_res     binson_error_dump();
binson_res     binson_error_clear_all();

#ifdef __cplusplus
}
#endif

#endif // BINSON_ERROR_H_INCLUDED
