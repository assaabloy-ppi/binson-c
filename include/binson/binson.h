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
 * \file binson.h
 * \brief Binson-c library public API header file
 *
 * \author Alexander Reshniuk
 * \date 20/11/2015
 *
 ***********************************************/

#ifndef BINSON_H_INCLUDED
#define BINSON_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdarg.h>

#include "binson_config.h"
#include "binson_common.h"
#include "binson_error.h"
#include "binson_io.h"
#include "binson_writer.h"
#include "binson_parser.h"


/**
 *  Binson DOM tree traversal type
 */
typedef enum
{
    BINSON_TRAVERSE_PREORDER  = 0,  /**< Process parents before children */
    BINSON_TRAVERSE_POSTORDER,      /**< Process children before parents  */
    BINSON_TRAVERSE_BOTHORDER       /**< Process parents before children and then one more time after all children */

} binson_traverse_method;

/**
 *  Forward declarations
 */
typedef struct binson_                      binson;

#ifndef binson_node_DEFINED
typedef struct binson_node_  binson_node;
# define binson_node_DEFINED
#endif

typedef struct binson_traverse_cb_status_   binson_traverse_cb_status;
typedef struct binson_traverse_cb_param_    binson_traverse_cb_param;

/**
 *  Binson DOM tree traversal direction
 */
typedef enum binson_traverse_dir_
{
    BINSON_TRAVERSE_DIR_UNKNOWN   = 0,
    BINSON_TRAVERSE_DIR_DOWN,    /**< Indicates last processed node was parent of current processing node */
    BINSON_TRAVERSE_DIR_UP,      /**< Indicates last processed node was child of current processing node */
    BINSON_TRAVERSE_DIR_RIGHT,   /**< Indicates last processed node was left neighbor of current processing node */
    BINSON_TRAVERSE_DIR_LEFT,    /**< Indicates last processed node was right neighbor of current processing node */
    BINSON_TRAVERSE_DIR_NONE     /**< Indicates \c BINSON_TRAVERSE_BOTHORDER processing of empty node */

} binson_traverse_dir;

/**
 *  General purpose binson library API calls
 */
uint32_t        binson_lib_get_version();
bool            binson_lib_is_compatible();

binson_res      binson_new( binson **pobj );
binson_res      binson_init( binson *obj, binson_writer *writer, binson_parser *parser, binson_io *error_io );
binson_res      binson_free( binson *obj );

/**
 *  Binson context getters/setters
 */
binson_node*    binson_get_root( binson *obj );
binson_writer*  binson_get_writer( binson *obj );
binson_parser*  binson_get_parser( binson *obj );
binson_res	binson_set_writer( binson *obj, binson_writer *pwriter );
binson_res	binson_set_parser( binson *obj, binson_parser *pparser );


/**
 *  Node/tree creation/removal
 */
binson_res  binson_node_add( binson *obj, binson_node *parent, binson_node_type node_type, const char* key, binson_node **dst, binson_value *tmp_val );

binson_res  binson_node_add_object_empty( binson *obj, binson_node *parent, const char* key,  binson_node **dst);
binson_res  binson_node_add_array_empty( binson *obj, binson_node *parent, const char* key, binson_node **dst );

binson_res  binson_node_add_boolean( binson *obj, binson_node *parent, const char* key, binson_node **dst, bool val );
binson_res  binson_node_add_integer( binson *obj, binson_node *parent, const char* key, binson_node **dst, int64_t val );
binson_res  binson_node_add_double( binson *obj, binson_node *parent, const char* key, binson_node **dst, double val );
binson_res  binson_node_add_str( binson *obj, binson_node *parent, const char* key, binson_node **dst, const char* val );
binson_res  binson_node_add_bytes( binson *obj, binson_node *parent, const char* key, binson_node **dst, uint8_t *src_ptr,  size_t src_size );

binson_res  binson_node_clone( binson *obj, binson_node *parent, binson_node **dst, binson_node *node, const char* new_key );
binson_res  binson_node_clone_tree( binson *obj, binson_node *node, const char* new_key );  /* not implemented */
binson_res  binson_node_remove( binson *obj, binson_node *node );

/**
 *  Serialization/deserialization
 */
binson_res  binson_serialize( binson *obj, binson_raw_size *psize );
binson_res  binson_deserialize( binson *obj, binson_node *parent, const char* key, bool validate_only );

/**
 *  Node level getters/setters
 */
binson_node_type      binson_node_get_type( binson_node *node );
const char*           binson_node_get_key( binson_node *node );
/*binson_value*      binson_node_get_val( binson_node *node );*/

binson_res            binson_node_get_boolean( binson_node *node, bool *pbool );
binson_res            binson_node_get_integer( binson_node *node, int64_t *pinteger );
binson_res            binson_node_get_double( binson_node *node, double *pdouble );
binson_res            binson_node_get_string( binson_node *node, char **ppstr );
binson_res            binson_node_get_bytes( binson_node *node, uint8_t **ppbytes, binson_raw_size *psize );


bool                  binson_node_is_leaf_type( binson_node *node );

/**
 *  Tree level getters
 */
int             binson_node_get_depth(  binson_node *node );
binson_node*    binson_node_get_parent( binson_node *node );
binson_node*    binson_node_get_prev( binson_node *node );
binson_node*    binson_node_get_next( binson_node *node );
binson_node*    binson_node_get_first_sibling( binson_node *node );
binson_node*    binson_node_get_last_sibling( binson_node *node );
binson_node*    binson_node_get_first_child( binson_node *node );
binson_node*    binson_node_get_last_child( binson_node *node );

binson_res      binson_node_get_child_by_idx( );
binson_res      binson_node_get_child_by_key( binson *obj, binson_node *parent, const char *key, binson_node **pnode );
binson_res      binson_node_get_sibling_count( );
binson_res      binson_node_get_child_count( );

/**
 *  Binson tree traversal API calls
 */
typedef binson_res (*binson_traverse_callback)(binson *obj, binson_node *node, binson_traverse_cb_status *status, void* param );

#ifdef DEBUG
binson_res    binson_cb_dump_debug( binson *obj, binson_node *node, binson_traverse_cb_status *status, void* param );
binson_res    binson_cb_remove( binson *obj, binson_node *node, binson_traverse_cb_status *status, void* param );
#endif

binson_res    binson_traverse( binson *obj, binson_node *root_node, binson_traverse_method t_method, int max_depth, \
                                            binson_traverse_callback cb, void* param );

binson_res    binson_traverse_first( binson *obj, binson_node *root_node, binson_traverse_method t_method, int max_depth, \
                                                  binson_traverse_callback cb, binson_traverse_cb_status *status, void* param );

binson_res    binson_traverse_next( binson_traverse_cb_status *status );
bool          binson_traverse_is_done( binson_traverse_cb_status *status );
binson_node*  binson_traverse_get_current_node( binson_traverse_cb_status *status );

#ifdef __cplusplus
}
#endif

#endif /* BINSON_H_INCLUDED */
