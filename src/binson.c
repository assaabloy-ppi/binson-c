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
 * \file binson.h
 * \brief Binson-c library public API implementation file
 *
 * \author Alexander Reshniuk
 * \date 20/11/2015
 *
 ***********************************************/

#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <assert.h>

#include "binson.h"
#include "binson_io.h"
#include "binson_util.h"
#include "binson_writer.h"
#include "binson_parser.h"

#define BINSON_VERSION_HEX    ((BINSON_MAJOR_VERSION << 16) |   \
                              (BINSON_MINOR_VERSION << 8)  |   \
                              (BINSON_MICRO_VERSION << 0))

//****************************************************************************
// Private constants
//****************************************************************************



//****************************************************************************
// Private structures
//****************************************************************************
// main binson object
typedef struct binson_ {

  binson_node_t  *root;

  binson_writer  *writer;
  binson_writer  *parser;
/*  binson_io       io_input;
  binson_io       io_output;
  binson_io       io_errors;
*/
  binson_strstack_t  str_stack;

} binson;

typedef char* binson_node_key_t;



typedef union _binson_node_val_composite_t {

    binson_node_t   *first_child;
    binson_node_t   *last_child;

} _binson_node_val_composite_t;


typedef union _binson_node_val_t {

    binson_node_val_composite_t  children;   // for ARRAY, OBJECT only

    bool      b_data;
    int64_t   i_data;
    double    d_data;

      // vector-like data stored in separate memory block, used for STRING, BYTES
      struct  v_data {  // vector data
         uint8_t*    ptr;   // strings are zero-terminated, 'size' field takes ending '\0' into account
         size_t      size;  // size in bytes of payload data in block, pointed by ptr, see section 2. of BINSON-SPEC-1
      };

} binson_node_val_t;


// each node (both terminal and nonterminal) is binson_node_t instance
typedef struct _binson_node_t {

    binson_node_type    type;

    binson_node_key_t   key;
    binson_node_val_t   val;


    binson_node_t   *parent;
    binson_node_t   *prev;
    binson_node_t   *next;


} binson_node_t;

typedef struct binson_traverse_cb_status_    // set by iterating function (caller)
{
    //initial parameters
    binson                    *obj;
    binson_node               *root_node;
    binson_traverse_method      t_method;
    binson_depth_t              max_depth;
    binson_traverse_callback_t  cb;
    void*                       param;

    // processing
    binson_node      *current_node;   // last visited node;

    binson_traverse_dir dir;    // traverse direction
    binson_child_num_t  child_num;  // current node is 'child_num's parent's child
    binson_depth_t      depth;  // curent node's depth

    bool                done;   // no more nodes to process;

} binson_traverse_cb_status;

// used to pass parameters to/from callbacks
typedef struct binson_traverse_cb_param_
{
    union in_param
    {
        binson_child_num  idx;
        binson_node_key   key;
        struct dump
        {
            binson_io_obj  *io;
            bool            to_json;   // flag to use single callback function for both dumping to raw and to JSON
        }
    };

    union out_param
    {
        binson_node       *node;
        binson_node_num    node_num;
        int                cmp_res;   // node key to str key comparison result
    };

} binson_traverse_cb_param;


//****************************************************************************
// Private helper functions forward declarations
//****************************************************************************

/*
binson_res  _binson_token_is_valid();  //
binson_res  _binson_token_get_size();  // get size in bytes required to store value
binson_res  _binson_token_get_type();  //

binson_res  _binson_node_get_key_ref();
binson_res  _binson_node_get_val_ref();
binson_res  _binson_node_get_val_ref();
binson_res  _binson_node_get_next_token_ref();

binson_res  _binson_node_add_empty( binson *obj, binson_node_t  *parent, binson_node_type node_type );

binson_res  _binson_token_update();  // shrink token if required
binson_res  _binson_token_store_empty( binson *obj, binson_node_t *node, binson_node_type node_type, size_t  *stored_bytes );

binson_res  _binson_raw_store(binson *obj, binson_raw_ref_t ref, void* src, size_t size);
*/

binson_res  _binson_node_add_empty( binson *obj, binson_node_t *parent, binson_node_type node_type, binson_node_key_t key, binson_node_t **dst );
binson_res  _binson_node_add( binson *obj, binson_node_t *parent, binson_node_type node_type, binson_node_key_t key, binson_node_t **dst, binson_node_val_t *tmp_val );
binson_res  _binson_node_copy_val( binson_node_type node_type, binson_node_val_t *dst_val, binson_node_val_t *src_val );

// tree traversal iteration callbacks (iterators).
binson_res _binson_cb_lookup_key( binson *obj, binson_node_t *node, binson_traverse_cb_status *status, void* param );
binson_res _binson_cb_lookup_idx( binson *obj, binson_node_t *node, binson_traverse_cb_status *status, void* param );
binson_res _binson_cb_count( binson *obj, binson_node_t *node, binson_traverse_cb_status *status, void* param );
binson_res _binson_cb_dump( binson *obj, binson_node_t *node, binson_traverse_cb_status *status, void* param );
binson_res _binson_cb_key_compare( binson *obj, binson_node_t *node, binson_traverse_cb_status *status, void* param );
binson_res _binson_cb_remove( binson *obj, binson_node_t *node, binson_traverse_cb_status *status, void* param );

binson_res  _binson_node_serialize_to_io( binson *obj, binson_node_t *node, binson_io *io );

/**< Library level APIs */


/********************************************//**
 * \brief Return current installed binson lib version
 *
 * \return uint32_t
 *
 ***********************************************/
uint32_t  binson_lib_get_version()
{
   return BINSON_VERSION_HEX;
}

/********************************************//**
 * \brief Check if headers matches binson lib version installed
 *
 * \param void
 * \return bool
 *
 ***********************************************/
bool  binson_lib_is_compatible(void)
{
    uint16_t major = binson_lib_get_version() >> 16;

    return ()

    return major == YL_VERSION_MAJOR;
    BINSON_RES_ERROR_LIB_VERSION

}


/** \brief
 *
 * \param obj binson*
 * \param in binson_io*
 * \param out binson_io*
 * \param err binson_io*
 * \return binson_res
 */
binson_res  binson_init( binson *obj, binson_io *in, binson_io *out, binson_io *err )
{
  binson_res res;

  obj->io_input = in;
  obj->io_output = out;
  obj->io_errors = err;

  res = binson_error_init( obj->io_errors );

  obj->root = NULL;

  return res;
}

//****************************************************************************
// Private helper functions implementation
//****************************************************************************
binson_res  _binson_node_add_empty( binson *obj, binson_node_t *parent, binson_node_type node_type, binson_node_key_t key, binson_node_t **dst )
{
  binson_node_t  *me;

  // arg's validation
  if (!key && parent && (parent->type != BINSON_TYPE_ARRAY))  // missing key for non-array parent
    return BINSON_RES_ERROR_ARG_WRONG;

   me = (binson_node_t *)calloc(sizeof(binson_node_t));

  if (!me)
    return BINSON_RES_ERROR_OUT_OF_MEMORY;

  // prepare our node structure
  me->type = node_type;
  me->parent = parent;


   // allocating and cloning key
   if (parent->type == BINSON_TYPE_ARRAY)  // no keys for ARRAY children
   {
     me->key = NULL;
   }
   else
   {
     me->key = (binson_node_key_t *)malloc(strlen(key)+1);
     strcpy(me->key, key);
   }

   // val field is zero initiated due to calloc call

  me->next = NULL;

  // connect new node to tree
  // [2DO] lexi order list insert instead of list add (for all parent types but ARRAY)
  if (parent->children->last_child)  // parent is not empty
  {
    me->prev = parent->children.last_child;
    parent->children.last_child->next = me;
    parent->children.last_child = me;
  }
  else // parent is  empty
  {
    me->prev = NULL;
    parent->children.last_child = parent->children.first_child = me;
  }

  if (dst)
   *dst = me;

  return BINSON_RES_OK;
}

/*--------------------------------------------------------------------------*/
// Adds node with prefilled value in binson_node_val_t struct
/*--------------------------------------------------------------------------*/
binson_res  _binson_node_add( binson *obj, binson_node_t *parent, binson_node_type node_type, binson_node_key_t key, binson_node_t **dst, binson_node_val_t *tmp_val )
{
  binson_node_t  *node_ptr = NULL;
  binson_res   res = _binson_node_add_empty( obj, parent, node_type, key, &node_ptr );

  if (!SUCCESS(res))
    return res;

  if (dst)
    *dst = node_ptr;

  res = _binson_node_copy_val( node_type, &node_ptr->val, tmp_val );

  if (!SUCCESS(res))
    return res;

  // just to be sure
  node_ptr->val.children.first_child = NULL;
  node_ptr->val.children.last_child = NULL;

  return BINSON_RES_OK;
}

/*--------------------------------------------------------------------------*/
// Copy 'binson_node_val_t' structure, allocating memory for STRING and BYTES
/*--------------------------------------------------------------------------*/
binson_res  _binson_node_copy_val( binson_node_type node_type, binson_node_val_t *dst_val, binson_node_val_t *src_val )
{
  memcpy( dst_val, src_val, sizeof(binson_node_val_t) );

  // allocate new blocks.  'v_data.size'  value in  'src_val' arg must be valid
  if (node_type == BINSON_TYPE_STRING || node_type == BINSON_TYPE_BYTES)
  {
    dst_val->v_data.ptr = (char*)malloc(src_val->v_data.size);

    if (!dst_val->v_data.ptr)
      return BINSON_RES_ERROR_OUT_OF_MEMORY;

    memcpy( dst_val->v_data.ptr, src_val->v_data.ptr, src_val->v_data.size );
  }
    return BINSON_RES_OK;
}

/*--------------------------------------------------------------------------*/
// Key lookup iterator (callback) to use with binson_traverse_*() functions
/*--------------------------------------------------------------------------*/
binson_res _binson_cb_lookup_key( binson *obj, binson_node_t *node, binson_traverse_cb_status *status, void* param )
{
  _binson_traverse_cb_param_t *p = (_binson_traverse_cb_param_t *)param;
   binson_child_num *requested_idx = p->in_param.idx;

   if (status->child_num == requested_idx)
   {
     p->out_param.node = node;
     status->done = true;  // this force calling function to stop iterating;
   }
   else
     p->out_param.node = NULL;

   return BINSON_RES_OK;
}



/*--------------------------------------------------------------------------*/
// Index lookup iterator (callback) to use with binson_traverse_*() functions
/*--------------------------------------------------------------------------*/
binson_res _binson_cb_lookup_idx( binson *obj, binson_node_t *node, binson_traverse_cb_status *status, void* param )
{
  _binson_traverse_cb_param_t *p = (_binson_traverse_cb_param_t *)param;
   binson_node_key_t *requested_key = p->in_param.key;

   if (!strcmp(requested_key, node->key))
   {
     p->out_param.node = node;
     status->done = true;  // this force calling function to stop iterating;
   }
   else
     p->out_param.node = NULL;

   return BINSON_RES_OK;
}

/*--------------------------------------------------------------------------*/
// Node count iterator (callback) to use with binson_traverse_*() functions
/*--------------------------------------------------------------------------*/
binson_res _binson_cb_count( binson *obj, binson_node_t *node, binson_traverse_cb_status *status, void* param )
{
  _binson_traverse_cb_param_t *p = (_binson_traverse_cb_param_t *)param;

  if (!status->done && status->current_node == status->root_node)  // initialize counter in case of 1st iteration
    p->out_param.node_num = 0;

  p->out_param.node_num++;

  return BINSON_RES_OK;
}

/*--------------------------------------------------------------------------*/
// Callback used to convert in-memory node to raw binson data or to it's JSON string representation
/*--------------------------------------------------------------------------*/
binson_res _binson_cb_dump( binson *obj, binson_node_t *node, binson_traverse_cb_status *status, void* param )
{
  // use arrays as maps to replace complex if/switch logic
  const uint8_t binson_int_map[] = {BINSON_SIG_INTEGER_8,   // for 0 bytes of int data
                                    BINSON_SIG_INTEGER_8,   // for 1 bytes of int data
                                    BINSON_SIG_INTEGER_16,  // for 2 bytes of int data
                                    BINSON_SIG_INTEGER_32,  // for 3 bytes of int data
                                    BINSON_SIG_INTEGER_32,  // for 4 bytes of int data
                                    BINSON_SIG_INTEGER_64,  // for 5 bytes of int data
                                    BINSON_SIG_INTEGER_64,  // for 6 bytes of int data
                                    BINSON_SIG_INTEGER_64,  // for 7 bytes of int data
                                    BINSON_SIG_INTEGER_64 } // for 8 bytes of int data


  const uint8_t binson_str_map[] = {BINSON_SIG_STRING_8,   // for 0 bytes of int data
                                    BINSON_SIG_STRING_8,   // for 1 bytes of int data
                                    BINSON_SIG_STRING_16,  // for 2 bytes of int data
                                    BINSON_SIG_STRING_32,  // for 3 bytes of int data
                                    BINSON_SIG_STRING_32}  // for 3 bytes of int data

  const uint8_t binson_bytes_map[] = {BINSON_SIG_BYTES_8,   // for 0 bytes of int data
                                      BINSON_SIG_BYTES_8,   // for 1 bytes of int data
                                      BINSON_SIG_BYTES_16,  // for 2 bytes of int data
                                      BINSON_SIG_BYTES_32,  // for 3 bytes of int data
                                      BINSON_SIG_BYTES_32}  // for 3 bytes of int data


  _binson_traverse_cb_param_t   *p = (_binson_traverse_cb_param_t *)param;
  binson_io               *io = p->in_param.dump.io;
  bool                          to_json = p->in_param.dump.to_json;

  binson_node_type               node_type = status->current_node->type;
  binson_res                   res = BINSON_RES_OK;

  char *s_objdump, *s_format, *s_value, *s_value2;


  if (node_type == BINSON_TYPE_UNKNOWN)
     return BINSON_RES_ERROR_TYPE_UNKNOWN;


  if (node_type == BINSON_TYPE_OBJECT || node_type == BINSON_TYPE_ARRAY)
  {
        switch (status->dir)
        {
          case BINSON_TRAVERSE_DIR_DOWN:
          case BINSON_TRAVERSE_DIR_RIGHT:
            if (to_json && status->dir == BINSON_TRAVERSE_DIR_RIGHT)
                binson_io_write_byte(io, (uint8_t)',' );

            binson_io_write_byte(io, (to_json)? ((node_type == BINSON_TYPE_OBJECT)? (uint8_t)'{' : (uint8_t)'[') :
                                                 ((node_type == BINSON_TYPE_OBJECT)? BINSON_SIG_OBJ_BEGIN : BINSON_SIG_ARRAY_BEGIN);
            break;

          case BINSON_TRAVERSE_DIR_UP:
            binson_io_write_byte(io, (to_json)? ((node_type == BINSON_TYPE_OBJECT)? (uint8_t)'}' : (uint8_t)']') :
                                                 ((node_type == BINSON_TYPE_OBJECT)? BINSON_SIG_OBJ_END : BINSON_SIG_ARRAY_END);
            break;

          case BINSON_TRAVERSE_DIR_UNKNOWN:
          default:
            res = BINSON_RES_TRAVERSAL_CB;
            break;
        }
        return res;
  }

  if (to_json)
  {
    // leasing some space at top of strstack for temp string manipulations
    binson_strstack_lease( obj->str_stack, &s_objdump, BINSON_JSON_OBJ_LENGTH_LIMIT );
    binson_strstack_lease( obj->str_stack, &s_format, 20);
    binson_strstack_lease( obj->str_stack, &s_value, BINSON_JSON_OBJ_LENGTH_LIMIT );

    s_format[0] = 0;  // init with zero-string
    s_value[0] = 0;
    s_objdump[0] = 0;


    // constructing element formatting string for sprintf()
    if (status->dir == BINSON_TRAVERSE_DIR_RIGHT) // we are n'th child.... need comma prefix
     strcpy(s_format, ",");

    if (node->parent.type == BINSON_TYPE_OBJECT)
     strcat(s_format, "\"%s\": ");

    if (node_type == BINSON_TYPE_STRING || node_type == BINSON_TYPE_BYTES) // needs double quotes
       strcat(s_format, "\"%s\"");
      else
       strcat(s_format, "%s");
  }

  // dumping node's value
  switch (node.type)
  {
    case BINSON_TYPE_BOOLEAN:
      strcpy(s_value, (to_json)? ((node->val.b_data)? "true" : "false") :
                                 ((node->val.b_data)? BINSON_SIG_TRUE : BINSON_SIG_FALSE));
    break;

    case BINSON_TYPE_INTEGER:
      if (to_json)
        sprintf(s_value, "%" PRId64, node.val.i_data);  // printing int64_t portable way
      else  // to raw binson format
      {
        uint8_t sig_bytes = binson_util_get_significant_bytes(node.val.i_data);
        int64_t i_le_data = BINSON_ARCH_IS_LE? node.val.i_data : binson_util_int64_swap(node.val.i_data);  // make it LE

        binson_io_write_byte(io, binson_int_map[sig_bytes]);  // write binson int signature
        binson_io_write(io, &i_le_data, sig_bytes);  // write raw int representation
      }
    break;

    case BINSON_TYPE_DOUBLE:
      if (to_json)
        sprintf(s_value, "%f", node.val.d_data);
      else  // to raw binson format
      {
         int64_t i_le_data = (int64_t)(BINSON_ARCH_IS_LE? node.val.d_data : binson_util_int64_swap((int64_t)node.val.d_data));  // make it LE
        binson_io_write_byte(io, BINSON_SIG_DOUBLE);  // write binson double signature
        binson_io_write(io, &i_le_data, sizeof(double));  // write raw double representation
      }
    break;

    case BINSON_TYPE_STRING:
      if (to_json)
        strcpy(s_value, node.val.v_data.ptr); // [2DO] UTF-8 escaping
      else   // to raw binson format
      {
        uint8_t sig_bytes = binson_util_get_significant_bytes( strlen(node.val.v_data.ptr) );

        binson_io_write_byte(io, binson_str_map[sig_bytes]);  // write binson string signature
        binson_io_write_str(io, node.val.v_data.ptr, false);  // string characters without trailing zero
      }
    break;

    case BINSON_TYPE_BYTES:
      if (to_json)
      {
        binson_strstack_lease( obj->str_stack, &s_value2, 6 );
        s_value2[0] = 0;

        for (int i=0; i<node.val.v_data.size; i++)  // dumping binson BYTES as JSON string
        {
          sprintf(s_value2, "0x%02x ", node.val.v_data.ptr+i);
          strcat(s_value, s_value2);
        }

        binson_strstack_release( obj->str_stack, &s_value2 );
      }
      else  // to raw binson format
      {
        uint8_t sig_bytes = binson_util_get_significant_bytes( (int64_t)node.val.v_data.size );

        binson_io_write_byte(io, binson_bytes_map[sig_bytes]);  // write binson bytes signature
        binson_io_write(io, node.val.v_data.ptr, node.val.v_data.size);
      }
    break;

   case: BINSON_TYPE_UNKNOWN:
   default:
     res = BINSON_RES_ERROR_TYPE_UNKNOWN;
     break;
  }


  if (to_json)
  {
    // generate final string
    sprintf(s_objdump, s_format, s_value);

    // write to output
    binson_io_write_str(io, s_objdump, false);

    // release strstacks
    binson_strstack_release(obj->str_stack, &s_value);
    binson_strstack_release(obj->str_stack, &s_format);
    binson_strstack_release(obj->str_stack, &s_objdump);
  }

  return res;
}

/*--------------------------------------------------------------------------*/
// Callback used to node key against str key passed via param
/*--------------------------------------------------------------------------*/
binson_res _binson_cb_key_compare( binson *obj, binson_node_t *node, binson_traverse_cb_status *status, void* param )
{
  _binson_traverse_cb_param_t   *p = (_binson_traverse_cb_param_t *)param;

  p->out_param.cmp_res = strcmp(node->key, p->in_param.key);  // is ok for UTF-8 strings since strcmp() preserves lexicographic order

  return BINSON_RES_OK;
}

/*--------------------------------------------------------------------------*/
// Callback used free node value's memory and finally node itself
/*--------------------------------------------------------------------------*/
binson_res _binson_cb_remove( binson *obj, binson_node_t *node, binson_traverse_cb_status *status, void* param )
{
  /**< frees node's key memory */
  if (node->key)
  {
     free(node->key);
     node->key = NULL;
  }

  /**< frees node's value memory */
  if (node->val.v_data.ptr)
  {
     free(node->val.v_data.ptr);
     node->val.v_data.ptr = NULL;
  }

  /**< frees node itself */
  free(node);

  return BINSON_RES_OK;
}

/********************************************//**
 * \brief  Remove subtree with specified node as root to binson_io
 *
 * \param obj binson*
 * \param node binson_node_t*
 * \param io binson_io
 * \return binson_res
 *
 ***********************************************/
binson_res  _binson_node_serialize_to_io( binson *obj, binson_node_t *node, binson_io *io )
{
   _binson_traverse_cb_param_t  param;

   param.in_param.io = io;
   param.in_param.to_json = false;

   return binson_traverse( obj, node, BINSON_TRAVERSE_BOTHORDER, BINSON_DEPTH_LIMIT, _binson_cb_dump, &param );
}


//****************************************************************************
// Public API calls implementation
//****************************************************************************

/*--------------------------------------------------------------------------*/
// Creates empty OBJECT node and connects it to specified parent
/*--------------------------------------------------------------------------*/
binson_res  binson_node_add_object_empty( binson *obj, binson_node_t *parent, binson_node_key_t key, binson_node_t **dst );
{
  return  _binson_node_add_empty( obj, parent, BINSON_TYPE_OBJECT, key, dst );
}


/*--------------------------------------------------------------------------*/
// Creates BOOLEAN node and connects it to specified parent
/*--------------------------------------------------------------------------*/
binson_res  binson_node_add_boolean( binson *obj, binson_node_t *parent, binson_node_key_t key, binson_node_t **dst, bool val )
{
  binson_node_val_t  tmp_val;

  tmp_val.b_data = val;
  return _binson_node_add( obj, parent, BINSON_TYPE_BOOLEAN, key, dst, &tmp_val );
}

/*--------------------------------------------------------------------------*/
// Creates INTEGER node and connects it to specified parent
/*--------------------------------------------------------------------------*/
binson_res  binson_node_add_integer( binson *obj, binson_node_t *parent,  binson_node_key_t key, binson_node_t **dst, int64_t val )
{
  binson_node_val_t  tmp_val;

  tmp_val.i_data = val;
  return _binson_node_add( obj, parent, BINSON_TYPE_INTEGER, key, dst, &tmp_val );
}

/*--------------------------------------------------------------------------*/
// Creates DOUBLE node and connects it to specified parent
/*--------------------------------------------------------------------------*/
binson_res  binson_node_add_double( binson *obj, binson_node_t *parent binson_node_key_t key, binson_node_t **dst, double val )
{
  binson_node_val_t  tmp_val;

  tmp_val.d_data = val;
  return _binson_node_add( obj, parent, BINSON_TYPE_DOUBLE, key, dst, &tmp_val );
}

/*--------------------------------------------------------------------------*/
// Creates STRING node and connects it to specified parent
/*--------------------------------------------------------------------------*/
binson_res  binson_node_add_str( binson *obj, binson_node_t *parent, binson_node_key_t key, binson_node_t **dst, const char* val )
{
  binson_node_val_t  tmp_val;

  tmp_val.v_data.ptr = val;
  tmp_val.v_data.size = strlen(val)+1;  // need to copy trailing zero also

  return _binson_node_add( obj, parent, BINSON_TYPE_STRING, key, dst, &tmp_val );
}

/*--------------------------------------------------------------------------*/
// Creates BYTES node and connects it to specified parent
/*--------------------------------------------------------------------------*/
binson_res  binson_node_add_bytes( binson *obj, binson_node_t *parent, binson_node_key_t key, binson_node_t **dst, uint8_t src_ptr,  size_t src_size )
{
  binson_node_val_t  tmp_val;

  tmp_val.v_data.ptr = src_ptr;
  tmp_val.v_data.size = src_size;

  return _binson_node_add( obj, parent, BINSON_TYPE_BYTES, key, dst, &tmp_val );
}

/*--------------------------------------------------------------------------*/
// Add new node which is a copy of specified node
/*--------------------------------------------------------------------------*/
binson_res  binson_node_clone( binson *obj, binson_node_t *parent, binson_node_t **dst, binson_node_t *node, const char* new_key )
{
  return  _binson_node_add( obj, parent, node->type, new_key, dst, node->val );
}

/*--------------------------------------------------------------------------*/
// Creates empty ARRAY node and connects it to specified parent
/*--------------------------------------------------------------------------*/
binson_res  binson_node_add_array_empty( binson *obj, binson_node_t *parent, binson_node_key_t key, binson_node_t **dst )
{
  return  _binson_node_add_empty( obj, parent, BINSON_TYPE_ARRAY, key, dst );
}


/********************************************//**
 * \brief Remove subtree with specified node as root
 *
 * \param obj binson*
 * \param node binson_node_t*
 * \return binson_res
 *
 ***********************************************/
binson_res  binson_node_remove( binson *obj, binson_node_t *node )
{
  return binson_traverse( obj, node, BINSON_TRAVERSE_POSTORDER, BINSON_DEPTH_LIMIT, _binson_cb_remove, NULL );
}

/********************************************//**
 * \brief Serialize subtree with specified node as root
 *         to default binson's output object
 *
 * \param obj binson*
 * \param node binson_node_t*
 * \return binson_res
 *
 ***********************************************/
binson_res  binson_node_serialize( binson *obj, binson_node_t *node )
{
  if (!obj || !node)
    return BINSON_RES_ERROR_ARG_WRONG;

  if (!obj->output)
    return BINSON_RES_ERROR_BROKEN_INT_STRUCT;

  return _binson_node_serialize_to_io( obj, node, obj->output );
}

/********************************************//**
 * \brief Serialize to memory buffer subtree with
 *        specified node as root using binson raw format
 *
 * \param obj binson*
 * \param node binson_node_t*
 * \param ptr uint8_t*
 * \param buf_size size_t
 * \return binson_res
 *
 ***********************************************/
binson_res  binson_node_serialize_to_buf( binson *obj, binson_node_t *node, uint8_t *ptr, size_t buf_size )
{
  _binson_traverse_cb_param_t  param;
  binson_io  io;

  _binson_io_attach_bytebuf( &io, ptr, buf_size );
  return _binson_node_serialize_to_io( obj, node, io );
}

/********************************************//**
 * \brief Serialize subtree to existing open FILE stream
 *
 * \param obj binson*
 * \param node binson_node_t*
 * \param output FILE*
 * \return binson_res
 *
 ***********************************************/
binson_res  binson_node_serialize_to_stream( binson *obj, binson_node_t *node, FILE *output )
{
  _binson_traverse_cb_param_t  param;
  binson_io  io;

  _binson_io_attach_stream( &io, output );
  return _binson_node_serialize_to_io( obj, node, io );
}

/********************************************//**
 * \brief Serialize subtree to existing open file descriptor
 *
 * \param obj binson*
 * \param node binson_node_t*
 * \param fd int
 * \return binson_res
 *
 ***********************************************/
binson_res  binson_node_serialize_to_fd( binson *obj, binson_node_t *node, int fd )
{
  _binson_traverse_cb_param_t  param;
  binson_io  io;

  _binson_io_attach_fd( &io, fd );
  return _binson_node_serialize_to_io( obj, node, io );
}

/********************************************//**
 * \brief Serialize subtree to file specified by path string
 *
 * \param path const char*
 * \return binson_res
 *
 ***********************************************/
binson_res  binson_node_serialize_to_file( binson *obj, binson_node_t *node, const char *path )
{
  binson_res res;

  _binson_traverse_cb_param_t  param;
  binson_io  io;

  res = _binson_io_open_file( &io, path );

  if (!SUCCESS(res))
    return res;

  return _binson_node_serialize_to_io( obj, node, io );
}

/********************************************//**
 * \brief Deserialize raw binson data from default binson's input object
 *
 * \param obj binson*
 * \param node binson_node_t*
 * \return binson_res
 *
 ***********************************************/
binson_res  binson_node_deserialize( binson *obj, binson_node_t *parent )
{
  return BINSON_RES_ERROR_NOT_SUPPORTED;
}

/********************************************//**
 * \brief Deserialize raw binson data from byte buffer
 *
 * \param obj binson*
 * \param node binson_node_t*
 * \param ptr uint8_t*
 * \param buf_size size_t
 * \return binson_res
 *
 ***********************************************/
binson_res  binson_node_deserialize_from_buf( binson *obj, binson_node_t *parent, uint8_t *ptr, size_t buf_size )
{
  return BINSON_RES_ERROR_NOT_SUPPORTED;
}

/********************************************//**
 * \brief Deserialize raw binson data from open FILE stream
 *
 * \param obj binson*
 * \param node binson_node_t*
 * \param output FILE*
 * \return binson_res
 *
 ***********************************************/
binson_res  binson_node_deserialize_from_stream( binson *obj, binson_node_t *parent, FILE *output )
{
  return BINSON_RES_ERROR_NOT_SUPPORTED;
}

/********************************************//**
 * \brief Deserialize raw binson data from open file descriptor
 *
 * \param obj binson*
 * \param node binson_node_t*
 * \param fd int
 * \return binson_res
 *
 ***********************************************/
binson_res  binson_node_deserialize_from_fd( binson *obj, binson_node_t *parent, int fd )
{
  return BINSON_RES_ERROR_NOT_SUPPORTED;
}

/********************************************//**
 * \brief Deserialize raw binson data from file specified by path string
 *
 * \param path const char*
 * \return binson_res
 *
 ***********************************************/
binson_res  binson_node_deserialize_from_file( binson *obj, binson_node_t *parent, const char *path )
{
  return BINSON_RES_ERROR_NOT_SUPPORTED;
}


/*--------------------------------------------------------------------------*/
// Begin tree traversal and process first available node
/*--------------------------------------------------------------------------*/
binson_res  binson_traverse_begin( binson *obj, binson_node_t *root_node, binson_traverse_method t_method, int max_depth, \
                                     binson_traverse_callback_t cb, binson_traverse_cb_status *status, void* param )
{
    binson_res res;

    if (status->done)
      return BINSON_RES_TRAVERSAL_DONE;

    if (obj && root_node)  // first iteration
    {
      // store parameters to status structure
      status->obj         = obj;
      status->root_node   = root_node;
      status->t_method    = t_method;
      status->max_depth   = max_depth;
      status->cb          = cb;
      status->param       = param;

      // initialize state variables
      status->current_node = root_node;

      status->dir         = BINSON_TRAVERSE_DIR_UNKNOWN;
      status->child_num   = 0;
      status->depth       = 0;
      status->done        = false;

      // check if we need to process root node at first iteration
      if (status->t_method != BINSON_TRAVERSE_POSTORDER)
        return status->cb( status->obj, status->current_node, status, status->param );
    }

    // non-first iteration.
//    if (binson_node_is_leaf_type(status->current_node))  // last processed was leaf
    if (status->dir == BINSON_TRAVERSE_DIR_UP ||
        status->depth >= status->max_depth ||
        status->current_node->val.children.first_child)  // there is no way down
    {
      if (status->current_node->next) // we can move right
      {
          status->current_node = status->current_node->next;  // update current node to it
          status->dir = BINSON_TRAVERSE_DIR_RIGHT;
          status->child_num++;  // keep track of index

          // processing moment doesn't depend on preorder/postorder for leaves
          return status->cb( status->obj, status->current_node, status, status->param );
      }
      else // no more neighbors from the right, moving up
      {
          if (status->t_method != BINSON_TRAVERSE_PREORDER)  // for postorder and 'bothorder'
            res = status->cb( status->obj, status->current_node, status, status->param );   // invoke callback for previous node

          if (status->current_node == status->root_node)  // can't move up, we are at root
          {
            status->done = true;
          }
          else  // next node to process will be parent of the current node
          {
            status->current_node = status->current_node->parent;
            status->dir         = BINSON_TRAVERSE_DIR_UP;
            status->depth--;
          }

           // ??? check need or not
          //if (status->t_method != BINSON_TRAVERSE_PREORDER)  // for postorder and 'bothorder'
          //  return status->cb( status->obj, status->current_node, status, status->param );
      }
    }
    else  // last processed has some children
    {
       status->current_node = status->current_node->val.children.first_child;   // select leftmost child
       status->dir         = BINSON_TRAVERSE_DIR_DOWN;
       status->child_num   = 0;
       status->depth++;

      if (status->t_method != BINSON_TRAVERSE_POSTORDER)
        return status->cb( status->obj, status->current_node, status, status->param );
    }

    return BINSON_RES_OK;
}

/*--------------------------------------------------------------------------*/
// Continue tree traversal and process next available
/*--------------------------------------------------------------------------*/
binson_res  binson_traverse_next( binson_traverse_cb_status *status )
{
    // to have code logic located in one place let's reuse 'binson_traverse_begin' function.
    // calling it with NULL pointers will make it work like 'binson_traverse_next'.
    return binson_traverse_begin( NULL, NULL, status->t_method, status->max_depth, status->cb, status, status->param);
}

/*--------------------------------------------------------------------------*/
// No more nodes to process?
/*--------------------------------------------------------------------------*/
bool  binson_traverse_is_done( binson_traverse_cb_status *status )
{
  return status->done;
}

/*--------------------------------------------------------------------------*/
// Which is current node being traversed?
/*--------------------------------------------------------------------------*/
binson_node_t*    binson_traverse_get_current_node( binson_traverse_cb_status *status )
{
  return status->current_node;
}

/*--------------------------------------------------------------------------*/
// Traverse all subtree with 'root_node' as subtree root
/*--------------------------------------------------------------------------*/
binson_res  binson_traverse( binson *obj, binson_node_t *root_node, binson_traverse_method t_method, int max_depth, \
                               binson_traverse_callback_t cb, void* param )
{
  binson_traverse_cb_status  status;
  binson_res res = binson_traverse_begin( obj, root_node, t_method, max_depth, cb, &status, param );

  while (!binson_traverse_is_done(&status))
  {
    res = binson_traverse_next(&status);
  };

  return res;
}

/*--------------------------------------------------------------------------*/
// Initialize new binson object
/*--------------------------------------------------------------------------*/
binson_res  binson_init( binson *obj )
{

  obj->is_le_arch = is_LE_arch();

  return BINSON_RES_OK;
}

/*--------------------------------------------------------------------------*/
// Free all memory used by binson object
/*--------------------------------------------------------------------------*/
binson_res  binson_free( binson *obj )

  binson_res res = BINSON_RES_OK;

  if (obj->root)
    res = binson_node_remove( obj, obj->root );

  if (obj->str_stack)
    binson_strstack_release( obj->str_stack );

  return res;
}

/*--------------------------------------------------------------------------*/
// Get root node pointer
/*--------------------------------------------------------------------------*/
binson_res  binson_get_root( binson *obj, binson_node_t  **node_ptr )
{
  if (obj == NULL || node_ptr == NULL)
    return BINSON_RES_ERROR_ARG_WRONG;

  node_ptr = obj->root;

  return BINSON_RES_OK;
}

/** \brief
 *
 * \param obj binson*
 * \return binson_error*
 */
binson_error*  binson_get_err( binson *obj )
{
  return binson->err;
}

/********************************************//**
 * \brief Get node type
 *
 * \param node binson_node_t*
 * \return binson_node_type
 *
 ***********************************************/
binson_node_type  binson_node_get_type( binson_node_t *node )
{
  return node->type;
}

/********************************************//**
 * \brief Get node key
 *
 * \param node binson_node_t*
 * \return binson_node_key_t*
 *
 ***********************************************/
binson_node_key_t*    binson_node_get_key( binson_node_t *node )
{
  return &node->key;
}

/********************************************//**
 * \brief Get node value struct pointer
 *
 * \param node binson_node_t*
 * \return binson_node_val_t*
 *
 ***********************************************/
binson_node_val_t*    binson_node_get_val( binson_node_t *node )
{
  return &node->val;
}

/** \brief
 *
 * \param node binson_node*
 * \return bool
 */
bool binson_node_is_leaf_type( binson_node *node )
{
  return (node->type == BINSON_TYPE_OBJECT || node->type == BINSON_TYPE_ARRAY)? false:true;
}

/********************************************//**
 * \brief Get node's parent node
 *
 * \param node binson_node_t*
 * \return binson_node_t
 *
 ***********************************************/
binson_node_t*   binson_node_get_parent( binson_node_t *node )
{
  return node->parent;
}

/********************************************//**
 * \brief Get depth level of the node. Depth of root is 0.
 *
 * \param node binson_node_t*
 * \return int
 *
 ***********************************************/
int  binson_node_get_depth(  binson_node_t *node )
{
   int depth = 0;

   while (node && node->parent)
   {
     node = node->parent;
     depth++;
   }

   return depth;
}

/********************************************//**
 * \brief Get previous (left) sibling of the node
 *
 * \param node binson_node_t*
 * \return binson_node_t
 *
 ***********************************************/
binson_node_t*    binson_node_get_prev( binson_node_t *node )
{
  return node->prev;
}

/********************************************//**
 * \brief  Get next (right) sibling of the node
 *
 * \param node binson_node_t*
 * \return binson_node_t
 *
 ***********************************************/
binson_node_t*    binson_node_get_next( binson_node_t *node )
{
  return node->next;
}

/********************************************//**
 * \brief  Get most left sibling of the node
 *
 * \param node binson_node_t*
 * \return binson_node_t
 *
 ***********************************************/
binson_node_t*  binson_node_get_first_sibling( binson_node_t *node )
{
   if (!node || !node->parent || (node == node->parent->val.children.first_child))
     return NULL;

   return node->parent->val.children.first_child;
}

/********************************************//**
 * \brief Get most right sibling of the node
 *
 * \param node binson_node_t*
 * \return binson_node_t
 *
 ***********************************************/
binson_node_t*  binson_node_get_last_sibling( binson_node_t *node )
{
  if (!node || !node->parent || (node == node->parent->val.children.last_child))
     return NULL;

   return node->parent->val.children.last_child;
}
}

/********************************************//**
 * \brief Get first child of the node
 *
 * \param node binson_node_t*
 * \return binson_node_t
 *
 ***********************************************/
binson_node_t*  binson_node_get_first_child( binson_node_t *node )
{
  if (!node || binson_node_is_leaf_type(node))
    return NULL;

  return node->val.children.first_child;
}

/********************************************//**
 * \brief Get last child of the node
 *
 * \param node binson_node_t*
 * \return binson_node_t
 *
 ***********************************************/
binson_node_t*  binson_node_get_last_child( binson_node_t *node )
{
  if (!node || binson_node_is_leaf_type(node))
    return NULL;

  return node->val.children.last_child;
}


/*--------------------------------------------------------------------------*/
// Add new empty OBJECT node to DOM
/*--------------------------------------------------------------------------*/



   switch (node_type)
   {






     case BINSON_TYPE_OBJECT:
       me->val.children.first_child = NULL;
       me->val.children.last_child = NULL;
     break;



#define   BINSON_TYPE_ARRAY     2
#define   BINSON_TYPE_BOOLEAN   3
#define   BINSON_TYPE_INTEGER   4
#define   BINSON_TYPE_DOUBLE    5
#define   BINSON_TYPE_STRING    6
#define   BINSON_TYPE_BYTES     7


     default:
       free(me->key);
       free(me);
     return BINSON_RES_ERROR_ARG_WRONG;
   }

   binson_node_key_t   val;
