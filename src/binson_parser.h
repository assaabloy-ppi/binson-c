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
 * \file binson_parser.h
 * \brief Binson binary format parsing API header file
 *
 * \author Alexander Reshniuk
 * \date 20/11/2015
 *
 ***********************************************/

#ifndef BINSON_PARSER_H_INCLUDED
#define BINSON_PARSER_H_INCLUDED

#include "binson_config.h"
#include "binson_error.h"
#include "binson_io.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  Forward declarations
 */
typedef struct binson_parser_  binson_parser;

#ifndef binson_node_DEFINED
typedef struct binson_node_  binson_node;
# define binson_node_DEFINED
#endif

/**
 *  Parsing callback declaration
 */
typedef binson_res (*binson_parser_cb)( binson_node *node, int depth, void* param );

/**
 *  Binson parser API calls
 */
binson_res  binson_parser_init( binson_parser *parser,  binson_io *io );
binson_res  binson_parser_free( binson_parser *parser );

binson_res  binson_parser_start( binson_parser *parser, binson_parser_cb cb, void* param );
binson_res  binson_parser_continue( binson_parser *parser, binson_parser_cb cb, void* param );

#ifdef __cplusplus
}
#endif

#endif /* BINSON_PARSER_H_INCLUDED */
