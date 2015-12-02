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
 * \file binson_writer.c
 * \brief Binson format writer implementation file
 *
 * \author Alexander Reshniuk
 * \date 20/11/2015
 *
 ***********************************************/

#include <string.h>

#include "binson_config.h"
#include "binson_writer.h"
#include "binson_util.h"

#include <assert.h>

/**< Binson binary format 8-bit signatures */
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

/**
 *  Binson writer context struct
 */
typedef struct binson_writer_
{
  binson_io*            io;                      /**< Associated \c binson_io struct */
  binson_writer_format  format;                  /**< Current binson writer output format */

#ifdef BINSON_WITH_JSON_OUTPUT
  /**
   * When moving to next level down (starting to write new OBJECT or ARRAY) it's required to store in stack
   * current horizontal item index to restore it on the return trip. This functionality implemented
   * for BINSON_WRITER_FORMAT_JSON_* modes only to keep track of commas on each level.
   */
  uint8_t               sig_stack[BINSON_DEPTH_LIMIT]; /**< Used to keep track of parents: OBJECT or ARRAY */
  binson_child_num      idx_stack[BINSON_DEPTH_LIMIT]; /**< Tracking indexes of children */
  int                   depth;                         /**< Current tree depth. Used as stack top pointer */
#endif

} binson_writer_;

/**<  \cond Private section (ignored by doxygen) begin */
#ifdef BINSON_WITH_JSON_OUTPUT

/**< JSON output Indention size for each nesting level */
#define BINSON_WRITER_INDENT_FACTOR    2

/** \brief Private helper. Build \c printf() formatting string for different data types, etc
 *
 * \param buf char*               Destination buffer pointer
 * \param parent_sig uint8_t      Use \c BINSON_SIG_OBJ_BEGIN to report paren is OBJECT
 * \param modifier char           Standard \c printf()'s type modifying character
 * \param need_quotes bool        Enclose value with quotes or not
 * \return void
 */
void build_json_fmt_str( char* buf, uint8_t parent_sig, char *modifier, bool need_quotes )
{
  sprintf(buf, "%s%s%%%s%s", (parent_sig == BINSON_SIG_OBJ_BEGIN)? "\"%s\": " : "",
                           (need_quotes)? "\"" : "", modifier, (need_quotes)? "\"" : "" );
}

/** \brief Private helper. Writes comma separator with indents for \c BINSON_WRITER_FORMAT_JSON_NICE format
 *
 * \param writer binson_writer*   Context
 * \return binson_res             Result code
 */
binson_res  write_separator( binson_writer *writer )
{
  int i;

  if (writer->idx_stack[writer->depth-1] > 0)
    binson_io_write_str( writer->io, ", ", true );

  if (writer->format == BINSON_WRITER_FORMAT_JSON_NICE)
  {
    binson_io_write_byte( writer->io, (uint8_t)'\n' );
    for (i=0; i<writer->depth*BINSON_WRITER_INDENT_FACTOR; i++)   /**<  Indent white spaces */
    {
      binson_io_write_byte( writer->io, (uint8_t)' ' );
    }
    binson_io_write_byte( writer->io, '\0' );
  }

  return BINSON_RES_OK;
}
#endif


/** \brief Private helper. Common code for writing OBJECT & ARRAY signatures
 *
 * \param writer binson_writer*       Context
 * \param sig uint8_t                 Signature to specify type of OBJECT
 * \return binson_res                 Result code
 */
binson_res  write_frame_sig( binson_writer *writer, uint8_t sig  )
{
  binson_res res = BINSON_RES_OK;

  /**< Initial parameter validation */
  if (!writer)
    return BINSON_RES_ERROR_ARG_WRONG;

#ifdef BINSON_WITH_JSON_OUTPUT
  /**< Updating tracking vars for new nesting level */
  if (sig == BINSON_SIG_OBJ_BEGIN || sig == BINSON_SIG_ARRAY_BEGIN)
  {
    writer->sig_stack[writer->depth] = sig;
    writer->depth++;
    writer->idx_stack[writer->depth] = 0;
  }
  else
  if (sig == BINSON_SIG_OBJ_END || sig == BINSON_SIG_ARRAY_END)
  {
    writer->depth--;
    writer->idx_stack[writer->depth]++;
  }
  else
  {
    writer->idx_stack[writer->depth]++;
  }
#endif

  switch (writer->format)
  {
    case BINSON_WRITER_FORMAT_RAW:
      res = binson_io_write_byte( writer->io, sig );
    break;

    case BINSON_WRITER_FORMAT_HEX:
      res = binson_io_write_str( writer->io, STR(sig) " ", true );
    break;

#ifdef BINSON_WITH_JSON_OUTPUT
    case BINSON_WRITER_FORMAT_JSON:
    case BINSON_WRITER_FORMAT_JSON_NICE:
      res = write_separator( writer );
      res = binson_io_write_str( writer->io, sig == BINSON_SIG_OBJ_BEGIN? "{ ": (sig == BINSON_SIG_OBJ_END? "} " :
                                            (sig == BINSON_SIG_ARRAY_BEGIN? "[ " : (sig == BINSON_SIG_ARRAY_END? "" : "] "))), true );
    break;
#endif

    default:
      return BINSON_RES_ERROR_ARG_WRONG;
  }

  return res;
}
/**<  \endcond Private section (ignored by doxygen) end */


/** \brief Initilize \c binson_writer context
 *
 * \param writer binson_writer*         Context
 * \param io binson_io*                 IO abstraction layer instance
 * \param format binson_writer_format   Output format
 * \return binson_res                   Result code
 */
binson_res  binson_writer_init( binson_writer *writer, binson_io *io, binson_writer_format format )
{
  /**< Initial parameter validation */
  if (!writer || !io || format < 0 || format >= BINSON_WRITER_FORMAT_LAST )
    return BINSON_RES_ERROR_ARG_WRONG;

  writer->io                 = io;
  writer->format             = format;
#ifdef BINSON_WITH_JSON_OUTPUT
  binson_writer_start( writer );
#endif

  return BINSON_RES_OK;
}

/** \brief Free all resources used by \c binson_writer instance
 *
 * \param writer binson_writer*   Context
 * \return binson_res             Result code
 */
binson_res  binson_writer_free( binson_writer *writer )
{
  /**< Initial parameter validation */
  if (!writer)
    return BINSON_RES_ERROR_ARG_WRONG;

  binson_writer_start( writer );   /**< Reset internal state */

  return BINSON_RES_OK;     /**< Do nothing since we had no mallocs */
}

/** \brief Set output format
 *
 * \param writer binson_writer*         Context
 * \param format binson_writer_format   Output format
 * \return binson_res                   Result code
 */
binson_res  binson_writer_set_format( binson_writer *writer, binson_writer_format format )
{
  /**< Initial parameter validation */
  if (!writer || format < 0 || format >= BINSON_WRITER_FORMAT_LAST)
    return BINSON_RES_ERROR_ARG_WRONG;

  writer->format = format;

  return BINSON_RES_OK;
}

/** \brief Set input/output abstraction layer instance
 *
 * \param writer binson_writer*   Context
 * \param io binson_io*           IO abstraction layer instance
 * \return binson_res             Result code
 */
binson_res  binson_writer_set_io( binson_writer *writer, binson_io *io )
{
  /**< Initial parameter validation */
  if (!writer || !io)
    return BINSON_RES_ERROR_ARG_WRONG;

  writer->io = io;

  return BINSON_RES_OK;
}

/** \brief Reset current state and start new writer session
 *
 * \param writer binson_writer*   Context
 * \return binson_res             Result code
 */
binson_res  binson_writer_start( binson_writer *writer )
{
  /**< Initial parameter validation */
  if (!writer)
    return BINSON_RES_ERROR_ARG_WRONG;

#ifdef BINSON_WITH_JSON_OUTPUT
  writer->depth              = 0;
  writer->idx_stack[0]       = 0;
  writer->sig_stack[0]       = 0;
#endif

  return BINSON_RES_OK;
}

/** \brief Write output for OBJECT begin
 *
 * \param writer binson_writer*   Context
 * \return binson_res             Result code
 */
binson_res  binson_writer_write_object_begin( binson_writer *writer )
{
  return write_frame_sig( writer, BINSON_SIG_OBJ_BEGIN );
}

/** \brief Write output for OBJECT end
 *
 * \param writer binson_writer*   Context
 * \return binson_res             Result code
 */
binson_res  binson_writer_write_object_end( binson_writer *writer )
{
  return write_frame_sig( writer, BINSON_SIG_OBJ_END );
}

/** \brief Write output for ARRAY begin
 *
 * \param writer binson_writer*   Context
 * \return binson_res             Result code
 */
binson_res  binson_writer_write_array_begin( binson_writer *writer )
{
  return write_frame_sig( writer, BINSON_SIG_ARRAY_BEGIN );
}

/** \brief Write output for ARRAY end
 *
 * \param writer binson_writer*   Context
 * \return binson_res             Result code
 */
binson_res  binson_writer_write_array_end( binson_writer *writer )
{
  return write_frame_sig( writer, BINSON_SIG_ARRAY_END );
}

/** \brief Write output to io for single bool value
 *
 * \param writer binson_writer*   Context
 * \param val bool                Value
 * \return binson_res             Result code
 */
binson_res  binson_writer_write_boolean( binson_writer *writer, bool val )
{
  binson_res res = BINSON_RES_OK;

  /**< Initial parameter validation */
  if (!writer)
    return BINSON_RES_ERROR_ARG_WRONG;

#ifdef BINSON_WITH_JSON_OUTPUT
  writer->idx_stack[writer->depth]++;
#endif

  switch (writer->format)
  {
    case BINSON_WRITER_FORMAT_RAW:
      res = binson_io_write_byte( writer->io, val? BINSON_SIG_TRUE : BINSON_SIG_FALSE );
    break;

    case BINSON_WRITER_FORMAT_HEX:
      res = binson_io_write_str( writer->io, val? STR(BINSON_SIG_TRUE)" " : STR(BINSON_SIG_FALSE)" ", true );
    break;

#ifdef BINSON_WITH_JSON_OUTPUT
    case BINSON_WRITER_FORMAT_JSON:
    case BINSON_WRITER_FORMAT_JSON_NICE:
      {
        char fmt[ sizeof("\"%s\": \"%s\"") ];

        res = write_separator( writer );
        build_json_fmt_str( fmt, writer->sig_stack[writer->depth-1], "s", false );
        res = binson_io_printf( writer->io, fmt, val? "true" : "false" );
      }
    break;
#endif

    default:
      return BINSON_RES_ERROR_ARG_WRONG;
  }

  return res;
}

/** \brief Write output to io for single \c int8_t .. \c int64_t value
 *         with automatic type downgrade according to real bytes used
 *
 * \param writer binson_writer*   Context
 * \param val int64_t             Integer argument
 * \return binson_res             Result code
 */
binson_res  binson_writer_write_integer( binson_writer *writer, int64_t val )
{
  const uint8_t binson_int_map[] = { BINSON_SIG_INTEGER_8,      /* for 0 bytes of int data */
                                     BINSON_SIG_INTEGER_8,      /* for 1 bytes of int data */
                                     BINSON_SIG_INTEGER_16,     /* for 2 bytes of int data */
                                     BINSON_SIG_INTEGER_32,     /* for 3 bytes of int data */
                                     BINSON_SIG_INTEGER_32,     /* for 4 bytes of int data */
                                     BINSON_SIG_INTEGER_64,     /* for 5 bytes of int data */
                                     BINSON_SIG_INTEGER_64,     /* for 6 bytes of int data */
                                     BINSON_SIG_INTEGER_64,     /* for 7 bytes of int data */
                                     BINSON_SIG_INTEGER_64 };   /* for 8 bytes of int data */
  binson_res  res = BINSON_RES_OK;
  uint8_t     bbuf[sizeof(int64_t)+1];
  uint8_t     bsize;
  int         i;

  /**< Initial parameter validation */
  if (!writer)
    return BINSON_RES_ERROR_ARG_WRONG;

#ifdef BINSON_WITH_JSON_OUTPUT
  writer->idx_stack[writer->depth]++;
#endif

  /**< Convert value to INTEGER primitive and store it in specified byte buffer */
  bsize = binson_util_pack_integer( val, &bbuf[1], true );
  bbuf[0] = binson_int_map[bsize];

  /**< Format dependent output */
  switch (writer->format)
  {
    case BINSON_WRITER_FORMAT_RAW:
      res = binson_io_write( writer->io, bbuf, bsize+1 );
      break;

    case BINSON_WRITER_FORMAT_HEX:
      for (i=0; i<bsize+1; i++)
        res = binson_io_printf( writer->io, "%02x ", bbuf[i] );
      break;

#ifdef BINSON_WITH_JSON_OUTPUT
    case BINSON_WRITER_FORMAT_JSON:
    case BINSON_WRITER_FORMAT_JSON_NICE:
      {
        char fmt[ sizeof("\"%s\": \"%s\"") ];

        res = write_separator( writer );
        build_json_fmt_str( fmt, writer->sig_stack[writer->depth-1], "ld", false );
        res = binson_io_printf( writer->io, fmt, val );    /**< \todo fix printing int64_t in C89 */
      }
    break;
#endif

    default:
      return BINSON_RES_ERROR_ARG_WRONG;
  }

  return res;
}

/** \brief Write output to io for single \c double value
 *
 * \param writer binson_writer*   Context
 * \param val double              Value
 * \return binson_res             Result code
 */
binson_res  binson_writer_write_double( binson_writer *writer, double val )
{
  binson_res  res = BINSON_RES_OK;
  uint8_t     bbuf[sizeof(double)+1];
  size_t      i;

 /**< Initial parameter validation */
  if (!writer)
    return BINSON_RES_ERROR_ARG_WRONG;

#ifdef BINSON_WITH_JSON_OUTPUT
  writer->idx_stack[writer->depth]++;
#endif

  binson_util_pack_double( val, &bbuf[1] );
  bbuf[0] = BINSON_SIG_DOUBLE;

  /**< Format dependent output */
  switch (writer->format)
  {
    case BINSON_WRITER_FORMAT_RAW:
      res = binson_io_write( writer->io, bbuf, sizeof(double)+1 );
      break;

    case BINSON_WRITER_FORMAT_HEX:
      for (i=0; i<sizeof(double)+1; i++)
        res = binson_io_printf( writer->io, "%02x ", &bbuf[i] );
      break;

#ifdef BINSON_WITH_JSON_OUTPUT
    case BINSON_WRITER_FORMAT_JSON:
    case BINSON_WRITER_FORMAT_JSON_NICE:
      {
        char fmt[ sizeof("\"%s\": \"%s\"") ];

        res = write_separator( writer );
        build_json_fmt_str( fmt, writer->sig_stack[writer->depth-1], "f", false );
        res = binson_io_printf( writer->io, fmt, val );
      }
    break;
#endif

    default:
      return BINSON_RES_ERROR_ARG_WRONG;
  }

  return res;
}

/** \brief Private helper. Single code logic for \c binson_writer_write_str() and \c binson_writer_write_bytes()
 *
 * \param writer binson_writer*   Context
 * \param src_ptr uint8_t*        Byte buffer
 * \param src_size size_t         Size of byte buffer
 * \param sig uint8_t             Signature to distinct STRING / BYTES
 * \return binson_res
 */
binson_res  write_bytes( binson_writer *writer, uint8_t *src_ptr,  size_t src_size, uint8_t sig )
{
  const uint8_t binson_str_map[] = {BINSON_SIG_STRING_8,    /* for 0 bytes of int data */
                                    BINSON_SIG_STRING_8,    /* for 1 bytes of int data */
                                    BINSON_SIG_STRING_16,   /* for 2 bytes of int data */
                                    BINSON_SIG_STRING_32,   /* for 3 bytes of int data */
                                    BINSON_SIG_STRING_32};  /* for 3 bytes of int data */

  const uint8_t binson_bytes_map[] = {BINSON_SIG_BYTES_8,   /* for 0 bytes of int data */
                                      BINSON_SIG_BYTES_8,   /* for 1 bytes of int data */
                                      BINSON_SIG_BYTES_16,  /* for 2 bytes of int data */
                                      BINSON_SIG_BYTES_32,  /* for 3 bytes of int data */
                                      BINSON_SIG_BYTES_32}; /* for 3 bytes of int data */

  binson_res  res = BINSON_RES_OK;
  uint8_t     bbuf[sizeof(int64_t)+1];
  size_t      bsize, i, j;

  /**< Initial parameter validation */
  if (!writer)
    return BINSON_RES_ERROR_ARG_WRONG;

#ifdef BINSON_WITH_JSON_OUTPUT
  writer->idx_stack[writer->depth]++;
#endif

  /**< Convert buffer size to INTEGER primitive and store it in specified byte buffer */
  bsize = binson_util_pack_integer( src_size, &bbuf[1], true );
  bbuf[0] = (sig == BINSON_SIG_STRING_8)? binson_str_map[bsize] : binson_bytes_map[bsize];

  /**< Format dependent output */
  switch (writer->format)
  {
    case BINSON_WRITER_FORMAT_RAW:
      res = binson_io_write( writer->io, bbuf, bsize+1 );       /**< Write signature + packed length */
      res = binson_io_write( writer->io, src_ptr, src_size );   /**< Write byte buffer */
      break;

    case BINSON_WRITER_FORMAT_HEX:
      for (i=0; i<bsize+1; i++)
        res = binson_io_printf( writer->io, "%02x ", bbuf[i] );

      for (j=0; j<src_size; j++)
        res = binson_io_printf( writer->io, "%02x ", src_ptr[j] );
      break;

#ifdef BINSON_WITH_JSON_OUTPUT
    case BINSON_WRITER_FORMAT_JSON:
    case BINSON_WRITER_FORMAT_JSON_NICE:
      {
        char fmt[ sizeof("\"%s\": \"%s\"") ];

        res = write_separator( writer );
        build_json_fmt_str( fmt, writer->sig_stack[writer->depth-1], "", false ); /**< Print optional first part but not value */
        res = binson_io_write_byte(writer->io, '\"');

        for (i=0; i<bsize+1; i++)
          res = binson_io_printf( writer->io, "%02x ", bbuf[i] );

        for (j=0; j<src_size; j++)
          res = binson_io_printf( writer->io, "%02x ", src_ptr[j] );

        res = binson_io_write_byte(writer->io, '\"');
      }
    break;
#endif

    default:
      return BINSON_RES_ERROR_ARG_WRONG;
  }

  return res;
}

/** \brief Write STRING object to io
 *
 * \param writer binson_writer*   Context
 * \param str const char*         Source string
 * \return binson_res             Result code
 */
binson_res  binson_writer_write_str( binson_writer *writer, const char* str )
{
  return write_bytes( writer, (uint8_t *)str,  strlen(str), BINSON_SIG_STRING_8 );
}

/** \brief Write BYTES object to io
 *
 * \param writer binson_writer*   Context
 * \param src_ptr uint8_t*        Byte buffer
 * \param src_size size_t         Size of data in byte buffer
 * \return binson_res             Result code
 */
binson_res  binson_writer_write_bytes( binson_writer *writer, uint8_t *src_ptr,  size_t src_size )
{
  return write_bytes( writer, src_ptr,  src_size, BINSON_SIG_BYTES_8 );
}
