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

#include "binson/binson.h"
#include "binson/binson_error.h"
#include "binson/binson_io.h"
#include "binson_util.h"
#include "binson/binson_writer.h"
#include "binson/binson_parser.h"
#include "binson_token_buf.h"

#define BINSON_VERSION_HEX    ((BINSON_MAJOR_VERSION << 16) |   \
                              (BINSON_MINOR_VERSION << 8)  |   \
                              (BINSON_MICRO_VERSION << 0))                                                          

/* Binson context type */
typedef struct binson_ {

  binson_node     *root;

  binson_writer   *writer;
  binson_parser   *parser;

  binson_io       *error_io;

} binson_;

/* each node (both terminal and nonterminal) is 'binson_node' instance */
typedef struct binson_node_ {

    /* tree navigation refs */
    binson_node       *parent;
    binson_node       *prev;
    binson_node       *next;
    binson_node       *first_child;
    binson_node       *last_child;

    /* payload */
    binson_node_type   type;
    char              *key;
    binson_value       val;

} binson_node_;

/*
 *
 */
typedef struct binson_traverse_cb_status_    /* set by iterating function (caller) */
{
    /* initial parameters */
    binson                         *obj;
    binson_node                    *root_node;
    binson_traverse_method          t_method;
    int                             max_depth;
    binson_traverse_callback        cb;
    void*                           param;

    /* processing */
    binson_node                    *current_node;      /* last visited node */
    binson_node                     current_node_copy; /* node copy to prevent problems in case of deleting node while iteraing */


    binson_traverse_dir             dir;             /* traverse direction */
    binson_child_num                child_num;       /*  current node is 'child_num's parent's child */
    int                             depth;           /*  curent node's depth */

    bool                            done;            /*  no more nodes to process; */

} binson_traverse_cb_status_;

/* used to pass parameters to/from callbacks */
typedef struct binson_traverse_cb_param_
{
    union in_param
    {
        binson_child_num  idx;
        const char*       key;
        binson_writer    *writer;
        binson_parser    *parser;

    } in_param;

    union out_param
    {
        binson_node       *node;
        binson_node_num    node_num;
        int                cmp_res;   /* node key to str key comparison result */

    } out_param;

} binson_traverse_cb_param_;


/* used by binson_cb_build() callback */
typedef struct binson_cb_build_param_
{
    binson                         *obj;
    binson_node                    *root_node;
    const char                     *top_key;

    binson_node                    *parent_last;  /* parent for last added node */

} binson_cb_build_param_;

/* private helper functions */
binson_res  binson_node_add_empty( binson *obj, binson_node *parent, binson_node_type node_type, const char* key, binson_node **dst );
binson_res  binson_node_copy_val( binson_node_type node_type, binson_value *dst_val, binson_value *src_val );
binson_res  binson_node_copy_val_from_raw( binson_node_type node_type, binson_value *dst_val, binson_raw_value *src_val );
binson_res  binson_node_attach( binson *obj, binson_node *parent, binson_node *new_node );
binson_res  binson_node_detach( binson *obj, binson_node *node );

/* tree traversal iteration callbacks (iterators) */
binson_res  binson_cb_lookup_key( binson *obj, binson_node *node, binson_traverse_cb_status *status, void* param );
binson_res  binson_cb_lookup_idx( binson *obj, binson_node *node, binson_traverse_cb_status *status, void* param );
binson_res  binson_cb_count( binson *obj, binson_node *node, binson_traverse_cb_status *status, void* param );
binson_res  binson_cb_dump( binson *obj, binson_node *node, binson_traverse_cb_status *status, void* param );
binson_res  binson_cb_key_compare( binson *obj, binson_node *node, binson_traverse_cb_status *status, void* param );
binson_res  binson_cb_remove( binson *obj, binson_node *node, binson_traverse_cb_status *status, void* param );

/* \brief Return version of libbinson.so installed
 *
 * \return uint32_t
 */
uint32_t  binson_lib_get_version()
{
   return BINSON_VERSION_HEX;
}

/* \brief Check if headers matches binson lib version installed
 *
 * \param void
 * \return bool
 */
bool  binson_lib_is_compatible()
{
    uint16_t major = binson_lib_get_version() >> 16;
    return (major == BINSON_MAJOR_VERSION)? true:false;
}

/* \brief
 *
 * \param pobj binson**
 * \return binson_res
 */
binson_res  binson_new( binson **pobj )
{
  /* Initial parameter validation */
  if (!pobj )
    return BINSON_RES_ERROR_ARG_WRONG;

  *pobj = (binson *)malloc(sizeof(binson_));

  return BINSON_RES_OK;
}

/* \brief Initialize binson context object
 *
 * \param obj binson*
 * \param writer binson_writer*
 * \param parser binson_parser*
 * \return binson_res
 */
binson_res  binson_init( binson *obj, binson_writer *writer, binson_parser *parser, binson_io *error_io )
{
  binson_res  res = BINSON_RES_OK;

  obj->root       = NULL;
  obj->writer     = writer;
  obj->parser     = parser;
  obj->error_io   = error_io;

  res = binson_error_init( obj->error_io );
  if (FAILED(res)) return res;

  /* add empty root OBJECT */
  res = binson_node_add_empty( obj, NULL, BINSON_TYPE_OBJECT, NULL, &(obj->root) );
  if (FAILED(res)) return res;

  return res;
}

/* \brief Free all memory used by binson object
 *
 * \param obj binson*
 * \return binson_res
 */
binson_res  binson_free( binson *obj )
{
  binson_res res = BINSON_RES_OK;

  /* Initial parameter validation */
  if (!obj)
    return BINSON_RES_ERROR_ARG_WRONG;

  if (obj->root)
    res = binson_node_remove( obj, obj->root );

  if (obj)
    free( obj );

  return res;
}


/* \brief Allocate storage and attach new empty node to binson DOM tree
 *
 * \param obj binson*
 * \param parent binson_node*
 * \param node_type binson_node_type
 * \param key const char*
 * \param dst binson_node**
 * \return binson_res
 */
binson_res  binson_node_add_empty( binson *obj, binson_node *parent, binson_node_type node_type, const char* key, binson_node **dst )
{
  binson_res res = BINSON_RES_OK;
  binson_node  *me;

  /* arg's validation */
  if (!obj || (!key && parent && (parent->type != BINSON_TYPE_ARRAY)))  /* missing key for non-array parent */
    return BINSON_RES_ERROR_ARG_WRONG;

   me = (binson_node*) calloc(1, sizeof(binson_node));

  if (!me)
    return BINSON_RES_ERROR_OUT_OF_MEMORY;

  /* prepare our node structure */
  me->type = node_type;

   /* allocating and cloning key */
   if (!parent || parent->type == BINSON_TYPE_ARRAY)  /* no keys for ARRAY children */
   {
     me->key = NULL;
   }
   else
   {
     me->key = (char*)malloc(strlen(key)+1);
     strcpy(me->key, key);
   }

  binson_node_attach( obj, parent, me );

  if (dst)
   *dst = me;

  return BINSON_RES_OK;
}

/* \brief Adds node with prefilled value in binson_value struct
 *
 * \param obj binson*
 * \param parent binson_node*
 * \param node_type binson_node_type
 * \param key const char*
 * \param dst binson_node**
 * \param tmp_val binson_value*
 * \return binson_res
 */
binson_res  binson_node_add( binson *obj, binson_node *parent, binson_node_type node_type, const char* key, binson_node **dst, binson_value *tmp_val )
{
  binson_node  *node_ptr = NULL;
  binson_res   res = binson_node_add_empty( obj, parent, node_type, key, &node_ptr );

  if (!SUCCESS(res))
    return res;

  if (dst)
    *dst = node_ptr;

  res = binson_node_copy_val( node_type, &(node_ptr->val), tmp_val );

  if (!SUCCESS(res))
    return res;

  /* just to be sure */
 /* node_ptr->first_child = NULL;
  node_ptr->last_child = NULL;
*/
  return BINSON_RES_OK;
}

/** \brief Copy 'binson_value' structure, allocating memory for STRING and BYTES
 *
 * \param node_type binson_node_type
 * \param dst_val binson_value*
 * \param src_val binson_value*
 * \return binson_res
 */
binson_res  binson_node_copy_val( binson_node_type node_type, binson_value *dst_val, binson_value *src_val )
{
  size_t  sz = 0;
  memcpy( dst_val, src_val, sizeof(binson_value) );

  if (node_type == BINSON_TYPE_STRING)
  {
    sz = strlen(src_val->str_val)+1;
    dst_val->str_val = (char*) malloc( sz );
    BINSON_ASSERT( dst_val->str_val );
    memcpy( dst_val->str_val, src_val->str_val, sz );
  }
  else
  if (node_type == BINSON_TYPE_BYTES)
  {
    sz = src_val->bbuf_val.bsize;
    dst_val->bbuf_val.bptr = (uint8_t*) malloc( sz );
    BINSON_ASSERT( dst_val->bbuf_val.bptr );
    memcpy( dst_val->bbuf_val.bptr, src_val->bbuf_val.bptr, sz );
    dst_val->bbuf_val.bsize = sz;
  }

  return BINSON_RES_OK;
}

/** \brief Translate 'binson_raw_value' structure to 'binson_value', converting raw strings
 *          to zero-terminated C-strings, automatically allocating memory required.
 *
 * \param node_type binson_node_type
 * \param dst_val binson_value*
 * \param src_val binson_value*
 * \return binson_res
 */
binson_res  binson_node_copy_val_from_raw( binson_node_type node_type, binson_value *dst_val, binson_raw_value *src_val )
{
  binson_res res = BINSON_RES_OK;

  switch (node_type)
  {
    case BINSON_TYPE_BOOLEAN:
      dst_val->bool_val = src_val->bool_val;  break;

    case BINSON_TYPE_INTEGER:
      dst_val->int_val = src_val->int_val;  break;

    case BINSON_TYPE_DOUBLE:
      dst_val->double_val = src_val->double_val;  break;

    case BINSON_TYPE_STRING:
      dst_val->str_val = (char*) malloc( src_val->bbuf_val.bsize + 1 );  /* dst string will contain zero terminator */
      BINSON_ASSERT( dst_val->str_val );
      memcpy( dst_val->str_val, src_val->bbuf_val.bptr, src_val->bbuf_val.bsize );
      dst_val->str_val[ src_val->bbuf_val.bsize ] = '\0';  /* string terminator */
    break;

    case BINSON_TYPE_BYTES:
      dst_val->bbuf_val.bptr = (uint8_t*) malloc( src_val->bbuf_val.bsize );
      BINSON_ASSERT( dst_val->bbuf_val.bptr );
      memcpy( dst_val->bbuf_val.bptr, src_val->bbuf_val.bptr, src_val->bbuf_val.bsize );
      dst_val->bbuf_val.bsize = src_val->bbuf_val.bsize;
    break;

    default:  /* skipping value copy for another node types are is not error case */
    break;
  }

  return res;
}

/* \brief
 *
 * \param obj binson*
 * \param parent binson_node*
 * \param new_node binson_node*
 * \return binson_res
 */
binson_res  binson_node_attach( binson *obj, binson_node *parent, binson_node *new_node )
{
  new_node->parent = parent;
  new_node->next = NULL;

  /* connect new node to tree */
  if (parent && parent->last_child)  /* parent is not empty */
  {
    /*new_node->prev = parent->last_child;
    parent->last_child->next = new_node;
    parent->last_child = new_node;*/
   binson_node  *pnode = parent->first_child;

   while (pnode)
   {
      if ( strcmp(new_node->key, pnode->key) < 0 )
        break;
      pnode = pnode->next;
   };

   if (pnode)  /* insert before pnode */
   {
     new_node->prev    = pnode->prev;
     new_node->next    = pnode;
     if (pnode->prev)  /* insert non first - in-between */
     {
       pnode->prev->next = new_node;
     }
     else
     {
       parent->first_child = new_node;  /* insert as first item */
     }

     pnode->prev       = new_node;
   }
   else /* insert last */
   {
     new_node->prev = parent->last_child;
     parent->last_child->next = new_node;
     parent->last_child = new_node;
   }

  }
  else /* parent is  empty */
  {
    new_node->prev = NULL;
    if (parent)
      parent->last_child = parent->first_child = new_node;
  }

  return BINSON_RES_OK;
}

/* \brief
 *
 * \param obj binson*
 * \param node binson_node*
 * \return binson_res
 */
binson_res  binson_node_detach( binson *obj, binson_node *node )
{
  if (!obj || !node)
    return BINSON_RES_ERROR_ARG_WRONG;

  if (node->parent && node->parent->last_child == node)
    node->parent->last_child = NULL;

  if (node->parent && node->parent->first_child == node)
    node->parent->first_child = NULL;

  if (node->prev)
    node->prev->next = node->next;

  if (node->next)
    node->next->prev = node->prev;

  node->parent = NULL;
  node->prev = NULL;
  node->next = NULL;

  return BINSON_RES_OK;
}

/* \brief Key lookup iterator (callback) to use with binson_traverse_*() functions
 *
 * \param obj binson*
 * \param node binson_node*
 * \param status binson_traverse_cb_status*
 * \param param void*
 * \return binson_res
 */
binson_res _binson_cb_lookup_key( binson *obj, binson_node *node, binson_traverse_cb_status *status, void* param )
{
   binson_traverse_cb_param *p = (binson_traverse_cb_param *)param;
   binson_child_num  requested_idx = p->in_param.idx;

   if (!obj || !node || !status || !param)
    return BINSON_RES_ERROR_ARG_WRONG;

   if (status->child_num == requested_idx)
   {
     p->out_param.node = node;
     status->done = true;  /* this force calling function to stop iterating; */
   }
   else
     p->out_param.node = NULL;

   return BINSON_RES_OK;
}

/* \brief Index lookup iterator (callback) to use with binson_traverse_*() functions
 *
 * \param obj binson*
 * \param node binson_node*
 * \param status binson_traverse_cb_status*
 * \param param void*
 * \return binson_res
 */
binson_res binson_cb_lookup_idx( binson *obj, binson_node *node, binson_traverse_cb_status *status, void* param )
{
   binson_traverse_cb_param *p = (binson_traverse_cb_param *)param;
   const char *requested_key = p->in_param.key;

   if (!obj || !node || !status || !param)
    return BINSON_RES_ERROR_ARG_WRONG;


   if (!strcmp(requested_key, node->key))
   {
     p->out_param.node = node;
     status->done = true;  /* this force calling function to stop iterating; */
   }
   else
     p->out_param.node = NULL;

   return BINSON_RES_OK;
}

/* \brief Node count iterator (callback) to use with binson_traverse_*() functions
 *
 * \param obj binson*
 * \param node binson_node*
 * \param status binson_traverse_cb_status*
 * \param param void*
 * \return binson_res
 */
binson_res binson_cb_count( binson *obj, binson_node *node, binson_traverse_cb_status *status, void* param )
{
  binson_traverse_cb_param *p = (binson_traverse_cb_param *)param;

   if (!obj || !node || !status || !param)
    return BINSON_RES_ERROR_ARG_WRONG;

  if (!status->done && status->current_node == status->root_node)  /* initialize counter in case of 1st iteration */
    p->out_param.node_num = 0;

  p->out_param.node_num++;

  return BINSON_RES_OK;
}

/* \brief Callback used to convert in-memory node to raw binson data or to it's JSON string representation
 *
 * \param obj binson*
 * \param node binson_node*
 * \param status binson_traverse_cb_status*
 * \param param void*
 * \return binson_res
 */
binson_res binson_cb_dump( binson *obj, binson_node *node, binson_traverse_cb_status *status, void* param )
{
  binson_traverse_cb_param *p = (binson_traverse_cb_param *)param;
  binson_res  res = BINSON_RES_OK;

  UNUSED(p);

/*printf("t=%d, d=%d\n", node->type, status->dir);*/

  if (!obj || !node || !status || !status->current_node )
    return BINSON_RES_ERROR_ARG_WRONG;

  switch (node->type)
  {
    case BINSON_TYPE_OBJECT:
      if (status->dir == BINSON_TRAVERSE_DIR_UP)
        res = binson_writer_write_object_end( obj->writer );
      else
        res = binson_writer_write_object_begin( obj->writer, node->key );
    break;

    case BINSON_TYPE_ARRAY:
      if (status->dir == BINSON_TRAVERSE_DIR_UP)
        res = binson_writer_write_array_end( obj->writer );
      else
        res = binson_writer_write_array_begin( obj->writer, node->key );
    break;

    case BINSON_TYPE_BOOLEAN:
      res = binson_writer_write_boolean( obj->writer, node->key, node->val.bool_val );
    break;

    case BINSON_TYPE_INTEGER:
      res = binson_writer_write_integer( obj->writer, node->key, node->val.int_val );
    break;

    case BINSON_TYPE_DOUBLE:
      res = binson_writer_write_double( obj->writer, node->key, node->val.double_val );
    break;

    case BINSON_TYPE_STRING:
      res = binson_writer_write_str( obj->writer, node->key, (const char*)(node->val.bbuf_val.bptr) );
    break;

    case BINSON_TYPE_BYTES:
      res = binson_writer_write_bytes( obj->writer, node->key, node->val.bbuf_val.bptr, node->val.bbuf_val.bsize );
    break;

    case BINSON_TYPE_UNKNOWN:
    default:
      res = BINSON_RES_ERROR_TYPE_UNKNOWN;
    break;
  }

  return res;
}

/* \brief Callback useful for debugging DOM tree traversal functionality
 *
 * \param
 * \param
 * \return
 */
#ifdef DEBUG
binson_res  binson_node_dump_debug( binson *obj, binson_node *node )
{
  const char* fmt = "[node=%p, type=%d, key=\"%s\", val=%x] : \n parent=%p, prev=%p, next=%p, fch=%p, lch=%p\n\n";
  binson_io   *io;

  if (obj)
  {
    io = binson_writer_get_io( obj->writer );
    return binson_io_printf( io, fmt, node, node->type, node->key, node->val.int_val,
                                      node->parent, node->prev, node->next, node->first_child, node->last_child );
  }
  else
    printf(fmt, node, node->type, node->key, node->val.int_val,
                                      node->parent, node->prev, node->next, node->first_child, node->last_child );
}

binson_res  binson_cb_dump_debug( binson *obj, binson_node *node, binson_traverse_cb_status *status, void* param )
{
  binson_traverse_cb_param *p = (binson_traverse_cb_param *)param;
  binson_res  res = BINSON_RES_OK;
  binson_io   *io = binson_writer_get_io( obj->writer );

  UNUSED(p);

  res = binson_io_printf( io, "-> #%02d, depth=%02d, dir=%02d : ", status->child_num, status->depth, status->dir );
  return binson_node_dump_debug( obj, status->current_node );
}
#endif

/* \brief Callback used to node key against str key passed via param
 *
 * \param obj binson*
 * \param node binson_node*
 * \param status binson_traverse_cb_status*
 * \param param void*
 * \return binson_res
 */
binson_res binson_cb_key_compare( binson *obj, binson_node *node, binson_traverse_cb_status *status, void* param )
{
  binson_traverse_cb_param   *p = (binson_traverse_cb_param *)param;

  if (!obj || !node || !status || !param)
    return BINSON_RES_ERROR_ARG_WRONG;

  p->out_param.cmp_res = strcmp(node->key, p->in_param.key);  /* is ok for UTF-8 strings since strcmp() preserves lexicographic order */

  return BINSON_RES_OK;
}

/* \brief Callback used free node value's memory and finally node itself
 *
 * \param obj binson*
 * \param node binson_node*
 * \param status binson_traverse_cb_status*
 * \param param void*
 * \return binson_res
 */
binson_res binson_cb_remove( binson *obj, binson_node *node, binson_traverse_cb_status *status, void* param )
{
  binson_traverse_cb_param   *p = (binson_traverse_cb_param *)param;
  UNUSED(p);

/*binson_cb_dump_debug( obj, node, status, param);*/

  if (!obj)
    return BINSON_RES_ERROR_ARG_WRONG;

  /* frees node's key memory */
  if (node->key)
  {
    free(node->key);
    node->key = NULL;
  }

  /* frees node's value memory */
  if (node->type == BINSON_TYPE_STRING && node->val.str_val)
  {
    free( node->val.str_val );
    node->val.str_val = NULL;
  }
  else
  if (node->type == BINSON_TYPE_BYTES && node->val.bbuf_val.bptr)
  {
    free( node->val.bbuf_val.bptr );
    node->val.bbuf_val.bptr = NULL;
  }

  /* frees node itself */
  free(node);

  return BINSON_RES_OK;
}

/* \brief Creates empty OBJECT node and connects it to specified parent
 *
 * \param obj binson*
 * \param parent binson_node*
 * \param key const char*
 * \param dst binson_node**
 * \return binson_res
 */
binson_res  binson_node_add_object_empty( binson *obj, binson_node *parent, const char* key, binson_node **dst )
{
  return  binson_node_add_empty( obj, parent, BINSON_TYPE_OBJECT, key, dst );
}

/* \brief Creates BOOLEAN node and connects it to specified parent
 *
 * \param obj binson*
 * \param parent binson_node*
 * \param key const char*
 * \param dst binson_node**
 * \param val bool
 * \return binson_res
 */
binson_res  binson_node_add_boolean( binson *obj, binson_node *parent, const char* key, binson_node **dst, bool val )
{
  binson_value  tmp_val;

  tmp_val.bool_val = val;
  return binson_node_add( obj, parent, BINSON_TYPE_BOOLEAN, key, dst, &tmp_val );
}

/* \brief Creates INTEGER node and connects it to specified parent
 *
 * \param obj binson*
 * \param parent binson_node*
 * \param key const char*
 * \param dst binson_node**
 * \param val int64_t
 * \return binson_res
 */
binson_res  binson_node_add_integer( binson *obj, binson_node *parent,  const char* key, binson_node **dst, int64_t val )
{
  binson_value  tmp_val;

  tmp_val.int_val = val;
  return binson_node_add( obj, parent, BINSON_TYPE_INTEGER, key, dst, &tmp_val );
}

/* \brief Creates DOUBLE node and connects it to specified parent
 *
 * \param obj binson*
 * \param key binson_node *parentconst char*
 * \param dst binson_node**
 * \param val double
 * \return binson_res
 */
binson_res  binson_node_add_double( binson *obj, binson_node *parent, const char* key, binson_node **dst, double val )
{
  binson_value  tmp_val;

  tmp_val.double_val = val;
  return binson_node_add( obj, parent, BINSON_TYPE_DOUBLE, key, dst, &tmp_val );
}

/* \brief Creates STRING node and connects it to specified parent
 *
 * \param
 * \param
 * \return
 */
binson_res  binson_node_add_str( binson *obj, binson_node *parent, const char* key, binson_node **dst, const char* val )
{
  binson_value  tmp_val;

  tmp_val.str_val = (char*)val;
  return binson_node_add( obj, parent, BINSON_TYPE_STRING, key, dst, &tmp_val );
}

/* \brief Creates BYTES node and connects it to specified parent
 *
 * \param obj binson*
 * \param parent binson_node*
 * \param key const char*
 * \param dst binson_node**
 * \param src_ptr uint8_t
 * \param src_size size_t
 * \return binson_res
 */
binson_res  binson_node_add_bytes( binson *obj, binson_node *parent, const char* key, binson_node **dst, uint8_t *src_ptr,  size_t src_size )
{
  binson_value  tmp_val;

  tmp_val.bbuf_val.bptr   = src_ptr;
  tmp_val.bbuf_val.bsize  = src_size;
  return binson_node_add( obj, parent, BINSON_TYPE_BYTES, key, dst, &tmp_val );
}

/* \brief Add new node which is a copy of specified node
 *
 * \param obj binson*
 * \param parent binson_node*
 * \param dst binson_node**
 * \param node binson_node*
 * \param new_key const char*
 * \return binson_res
 */
binson_res  binson_node_clone( binson *obj, binson_node *parent, binson_node **dst, binson_node *node, const char* new_key )
{
  return  binson_node_add( obj, parent, node->type, new_key, dst, &node->val );
}

/* \brief Creates empty ARRAY node and connects it to specified parent
 *
 * \param obj binson*
 * \param parent binson_node*
 * \param key const char*
 * \param dst binson_node**
 * \return binson_res
 */
binson_res  binson_node_add_array_empty( binson *obj, binson_node *parent, const char* key, binson_node **dst )
{
  return  binson_node_add_empty( obj, parent, BINSON_TYPE_ARRAY, key, dst );
}

/** \brief Remove subtree with specified node as root
 *
 * \param obj binson*
 * \param node binson_node*
 * \return binson_res
 */
binson_res  binson_node_remove( binson *obj, binson_node *node )
{
  binson_res   res = BINSON_RES_OK;
  binson_node *parent, *prev, *next;

  if (!obj || !node)
    return BINSON_RES_ERROR_ARG_WRONG;

  /* save ptrs  because tree traversal will break subtree */
  parent = node->parent;
  prev   = node->prev;
  next   = node->next;

  res = binson_traverse( obj, node, BINSON_TRAVERSE_POSTORDER, BINSON_DEPTH_LIMIT, binson_cb_remove, NULL );  /* free all subtree mallocs */

  if (!parent)
  {
    obj->root = NULL;
  }
  else
  {
    if (prev && next)
      prev->next = next;
      next->prev = prev;

    if (!prev)
      parent->first_child = next;

    if (!next)
      parent->last_child = prev;
  }

  return res;
}

/* \brief
 *
 * \param obj binson*
 * \return binson_res
 *
 */
binson_res  binson_serialize( binson *obj )
{
  return binson_traverse( obj, binson_get_root(obj), BINSON_TRAVERSE_BOTHORDER, BINSON_DEPTH_LIMIT, binson_cb_dump, NULL );
}

/**
 *  Called by parser for each token group, used to build binson model
 */
binson_res binson_cb_build( binson_parser *parser, uint8_t token_cnt, binson_token_buf *tbuf, void* param )
{
  binson_cb_build_param_  *p = (binson_cb_build_param_ *)param;
  binson_node             *new_node = NULL;
  binson_raw_value         raw_key, raw_val;
  binson_res               res = BINSON_RES_OK;
  binson_node_type         node_type;
  bool                     is_closing_token;  /* true, if current token is final part of OBJECT/ARRAY */

  memset(&raw_key, 0, sizeof(binson_raw_value));
  memset(&raw_val, 0, sizeof(binson_raw_value));

  if (token_cnt > 1)
  {
    res = binson_token_buf_get_node_type( tbuf, 1, &node_type, &is_closing_token );
    res = binson_token_buf_get_token_payload( tbuf, 0, &raw_key );
    res = binson_token_buf_get_token_payload( tbuf, 1, &raw_val );
  }
  else
  {
   res = binson_token_buf_get_node_type( tbuf, 0, &node_type, &is_closing_token );
   if (node_type != BINSON_TYPE_OBJECT && node_type != BINSON_TYPE_ARRAY)
   {
      res = binson_token_buf_get_token_payload( tbuf, 0, &raw_val );
      raw_key.bbuf_val.bsize = 0;  /* make sure raw_key is empty */
   }
  }

  if (!is_closing_token)
  {
    /* allocating new node structure */
    new_node = (binson_node *)calloc(1, sizeof(binson_node));
    new_node->type = node_type;

     /* allocating and cloning key */
     if (!p->parent_last || p->parent_last->type == BINSON_TYPE_ARRAY)  /* no parent or ARRAY */
     {
       new_node->key = NULL;
     }
     else if (!raw_key.bbuf_val.bsize)  /* parent is OBJECT but we have no parsed key, so key from argument  */
     {
       new_node->key = (char*)malloc( strlen(p->top_key)+1 );
       strcpy(new_node->key, p->top_key);
     }
     else /* use key from parser */
     {
       new_node->key = (char*)malloc( raw_key.bbuf_val.bsize+1 );
       memcpy(new_node->key, raw_key.bbuf_val.bptr, raw_key.bbuf_val.bsize );
       new_node->key[ raw_key.bbuf_val.bsize ] = '\0';
     }

    if (p->parent_last)
    {
      res = binson_node_copy_val_from_raw( node_type, &(new_node->val), &raw_val );
      res = binson_node_attach( p->obj, p->parent_last, new_node );
    }
    else  /* deserialization which replace whole DOM tree */
    {
      if ( p->obj->root)
          res = binson_node_remove( p->obj, p->obj->root );

      /*free(p->obj->root->key);
      free(p->obj->root);*/

      p->obj->root = new_node;

    }
  } /* if (!is_closing_token) ... */

  if (node_type == BINSON_TYPE_ARRAY || node_type == BINSON_TYPE_OBJECT)
    p->parent_last = is_closing_token? p->parent_last->parent : new_node;

/*if (new_node)
binson_node_dump_debug( NULL, new_node );
else
  printf("new parent = %p  --\n", p->parent_last);
*/

  return res;
}


/** \brief
 *
 * \param obj binson*
 * \param parent binson_node*   If NULL, replaces whole DOM tree
 * \param key const char*       Used if parent is OBJECT, otherwise ignored
 * \param validate_only bool
 * \return binson_res
 */
binson_res  binson_deserialize( binson *obj, binson_node *parent, const char* key, bool validate_only )
{
  binson_cb_build_param_  param;
  binson_res              res;

  if (!obj)
    return BINSON_RES_ERROR_ARG_WRONG;

  UNUSED(validate_only);

  param.obj          = obj;
  param.root_node    = parent;
  param.parent_last  = parent; /*obj->root;*/
  param.top_key      = key;

  res = binson_parser_parse( obj->parser, binson_cb_build,  &param );

  return res;
}

/* \brief Begin tree traversal and process first available node
 *
 * \param
 * \param
 * \return
 */
binson_res  binson_traverse_begin( binson *obj, binson_node *root_node, binson_traverse_method t_method, int max_depth, \
                                     binson_traverse_callback cb, binson_traverse_cb_status *status, void* param )
{
    binson_res res = BINSON_RES_OK;

    if (status && obj && root_node)  /* first iteration */
    {
      /* store parameters to status structure */
      status->obj         = obj;
      status->root_node   = root_node;
      status->t_method    = t_method;
      status->max_depth   = max_depth;
      status->cb          = cb;
      status->param       = param;

      /* initialize state variables */
      status->current_node = root_node;

      status->dir         = BINSON_TRAVERSE_DIR_UNKNOWN;
      status->child_num   = 0;
      status->depth       = 0;
      status->done        = false;

      /* required for tree deletion to prevent sawing one's bough */
      memcpy( &status->current_node_copy, status->current_node, sizeof(binson_node) );

      /* check if we need to process root node at first iteration */
      if (status->t_method != BINSON_TRAVERSE_POSTORDER)
        return status->cb( status->obj, status->current_node, status, status->param );
    }

    if (status && status->done)
      return BINSON_RES_TRAVERSAL_DONE;

    /* non-first iteration. */
    if (status->dir == BINSON_TRAVERSE_DIR_UP ||
        status->depth >= status->max_depth ||
        status->current_node_copy.first_child == NULL)  /* there is no way down */
    {
      if (status->current_node && status->current_node_copy.next) /* we can move right */
      {
          status->current_node = status->current_node_copy.next;  /* update current node to it */
          status->dir = BINSON_TRAVERSE_DIR_RIGHT;
          status->child_num++;  /* keep track of index */

          /* required for tree deletion to prevent sawing one's bough */
          memcpy( &status->current_node_copy, status->current_node, sizeof(binson_node) );

          /* processing moment doesn't depend on preorder/postorder for leaves */
          return status->cb( status->obj, status->current_node, status, status->param );
      }
      else /* no more neighbors from the right, moving up */
      {
        bool empty_container = false;

        if ((status->current_node_copy.type == BINSON_TYPE_OBJECT || status->current_node_copy.type == BINSON_TYPE_ARRAY) &&
            status->current_node_copy.first_child == NULL && status->t_method != BINSON_TRAVERSE_PREORDER )
          empty_container = true;

        status->dir          = BINSON_TRAVERSE_DIR_UP;

        /* process closing part of empty OBJECT/ARRAY */
        if (status->depth > 0 && empty_container && status->t_method == BINSON_TRAVERSE_BOTHORDER)
          res = status->cb( status->obj, status->current_node, status, status->param );


        if (status->current_node_copy.parent)  /* root node case - prevent NULL assignment */
          status->current_node = status->current_node_copy.parent;


        if (!empty_container)  /* empty containers process on same depth level */
        {
           status->depth--;
           if (status->depth < 0)
           {
             status->done = true;
             return res;
           }
        }

        /* required for tree deletion to prevent sawing one's bough */
        memcpy( &status->current_node_copy, status->current_node, sizeof(binson_node) );

        if (status->t_method != BINSON_TRAVERSE_PREORDER)
          res = status->cb( status->obj, status->current_node, status, status->param );

        if (empty_container)   /* change depth after empty container processed */
        {
           status->depth--;
           if (status->depth < 0)
           {
             status->done = true;
             return res;
           }
        }
      }
    }
    else  /* last processed has some children */
    {
       status->current_node = status->current_node_copy.first_child;   /* select leftmost child */
       status->dir          = BINSON_TRAVERSE_DIR_DOWN;
       status->child_num    = 0;
       status->depth++;

       /* required for tree deletion to prevent sawing one's bough */
       memcpy( &status->current_node_copy, status->current_node, sizeof(binson_node) );

      if (status->t_method != BINSON_TRAVERSE_POSTORDER || status->current_node_copy.first_child == NULL)
        return status->cb( status->obj, status->current_node, status, status->param );
    }

    return res;
}

/* \brief Continue tree traversal and process next available
 *
 * \param status binson_traverse_cb_status*
 * \return binson_res
 */
binson_res  binson_traverse_next( binson_traverse_cb_status *status )
{
    /* to have code logic located in one place let's reuse 'binson_traverse_begin' function.
    / calling it with NULL pointers will make it work like 'binson_traverse_next'. */
    return binson_traverse_begin( NULL, NULL, status->t_method, status->max_depth, status->cb, status, status->param);
}

/* \brief No more nodes to process?
 *
 * \param status binson_traverse_cb_status*
 * \return bool
 */
bool  binson_traverse_is_done( binson_traverse_cb_status *status )
{
  return status->done;
}

/* \brief Which is current node being traversed?
 *
 * \param status binson_traverse_cb_status*
 * \return binson_node*
 */
binson_node*    binson_traverse_get_current_node( binson_traverse_cb_status *status )
{
  return status->current_node;
}

/* \brief Traverse all subtree with 'root_node' as subtree root
 *
 * \param
 * \param
 * \return
 */
binson_res  binson_traverse( binson *obj, binson_node *root_node, binson_traverse_method t_method, int max_depth, \
                               binson_traverse_callback cb, void* param )
{
  binson_traverse_cb_status  status;
  binson_res res = binson_traverse_begin( obj, root_node, t_method, max_depth, cb, &status, param );

  while (!binson_traverse_is_done( &status ))
    res = binson_traverse_next( &status );

  return res;
}

/** \brief  Get root node pointer
 *
 * \param obj binson*
 * \return binson_node*
 */
binson_node* binson_get_root( binson *obj )
{
  if (!obj)
    return NULL;

  return obj->root;
}

/* \brief Get node type
 *
 * \param node binson_node*
 * \return binson_node_type
 */
binson_node_type  binson_node_get_type( binson_node *node )
{
  return node->type;
}

/* \brief Get node key
 *
 * \param node binson_node*
 * \return const char*
 */
const char*    binson_node_get_key( binson_node *node )
{
  return node->key;
}

/* \brief Get node value struct pointer
 *
 * \param node binson_node*
 * \return binson_value*
 */
binson_value*    binson_node_get_val( binson_node *node )
{
  return &node->val;
}

/** \brief
 *
 * \param node binson_node*
 * \param pbool bool*
 * \return binson_res
 */
binson_res  binson_node_get_boolean( binson_node *node, bool *pbool )
{
  if (!node || !pbool)
    return BINSON_RES_ERROR_ARG_WRONG;

  *pbool = node->val.bool_val;

  return BINSON_RES_OK;
}

/** \brief
 *
 * \param node binson_node*
 * \param pinteger int64_t*
 * \return binson_res
 */
binson_res  binson_node_get_integer( binson_node *node, int64_t *pinteger )
{
  if (!node || !pinteger)
    return BINSON_RES_ERROR_ARG_WRONG;

  *pinteger = node->val.int_val;

  return BINSON_RES_OK;
}

/** \brief
 *
 * \param node binson_node*
 * \param pdouble double*
 * \return binson_res
 */
binson_res  binson_node_get_double( binson_node *node, double *pdouble )
{
  if (!node || !pdouble)
    return BINSON_RES_ERROR_ARG_WRONG;

  *pdouble = node->val.double_val;

  return BINSON_RES_OK;
}

/** \brief
 *
 * \param node binson_node*
 * \param pstr char**
 * \return binson_res
 */
binson_res  binson_node_get_string( binson_node *node, char **ppstr )
{
  if (!node || !ppstr)
    return BINSON_RES_ERROR_ARG_WRONG;

  *ppstr = node->val.str_val;

  return BINSON_RES_OK;
}

/** \brief
 *
 * \param node binson_node*
 * \param ppbytes uint8_t**
 * \param psize binson_raw_size*
 * \return binson_res
 */
binson_res  binson_node_get_bytes( binson_node *node, uint8_t **ppbytes, binson_raw_size *psize )
{
  if (!node || !ppbytes || !psize)
    return BINSON_RES_ERROR_ARG_WRONG;

  *ppbytes = node->val.bbuf_val.bptr;
  *psize   = node->val.bbuf_val.bsize;

  return BINSON_RES_OK;
}

/* \brief
 *
 * \param node binson_node*
 * \return bool
 */
bool binson_node_is_leaf_type( binson_node *node )
{
  return (node->type == BINSON_TYPE_OBJECT || node->type == BINSON_TYPE_ARRAY)? false:true;
}

/* \brief Get node's parent node
 *
 * \param node binson_node*
 * \return binson_node*
 */
binson_node*   binson_node_get_parent( binson_node *node )
{
  return node->parent;
}

/* \brief Get depth level of the node. Depth of root is 0.
 *
 * \param node binson_node*
 * \return int
 */
int  binson_node_get_depth(  binson_node *node )
{
   int depth = 0;

   while (node && node->parent)
   {
     node = node->parent;
     depth++;
   }

   return depth;
}

/* \brief Get previous (left) sibling of the node
 *
 * \param node binson_node*
 * \return binson_node*
 */
binson_node*    binson_node_get_prev( binson_node *node )
{
  return node->prev;
}

/* \brief Get next (right) sibling of the node
 *
 * \param node binson_node*
 * \return binson_node*
 */
binson_node*    binson_node_get_next( binson_node *node )
{
  return node->next;
}

/* \brief Get most left sibling of the node
 *
 * \param node binson_node*
 * \return binson_node*
 */
binson_node*  binson_node_get_first_sibling( binson_node *node )
{
   if (!node || !node->parent || (node == node->parent->first_child))
     return NULL;

   return node->parent->first_child;
}

/* \brief Get most right sibling of the node
 *
 * \param node binson_node*
 * \return binson_node*
 */
binson_node*  binson_node_get_last_sibling( binson_node *node )
{
  if (!node || !node->parent || (node == node->parent->last_child))
     return NULL;

   return node->parent->last_child;
}

/* \brief Get first child of the node
 *
 * \param node binson_node*
 * \return binson_node*
 */
binson_node*  binson_node_get_first_child( binson_node *node )
{
  if (!node || binson_node_is_leaf_type(node))
    return NULL;

  return node->first_child;
}

/* \brief Get last child of the node
 *
 * \param node binson_node*
 * \return binson_node*
 */
binson_node*  binson_node_get_last_child( binson_node *node )
{
  if (!node || binson_node_is_leaf_type(node))
    return NULL;

  return node->last_child;
}


/** \brief
 *
 * \param obj binson*
 * \param parent binson_node*
 * \param key const char*
 * \param pnode binson_node**
 * \return binson_res
 */
binson_res  binson_node_get_child_by_key( binson *obj, binson_node *parent, const char *key, binson_node **pnode )
{
  binson_node *node;

  if (!parent)
    parent = obj->root;

  node = parent->first_child;
  *pnode = NULL;

  while (node)
  {
    if ( !strcmp(key, node->key) )
    {
      *pnode = node;
       break;
    }
    node = node->next;
  }

  return BINSON_RES_OK;
}
