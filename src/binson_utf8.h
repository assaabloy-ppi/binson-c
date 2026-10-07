/*
 *  Copyright (c) 2015 ASSA ABLOY AB
 *
 *  This file is part of binson-c, BINSON serialization format library in C.
 *
 *  SPDX-License-Identifier: MIT
 *  See the LICENSE file in the project root for the full license text.
 */

/********************************************//**
 * \file binson_utf8.h
 * \brief UTF-8 utility functions header file
 *
 * \author Alexander Reshniuk
 * \date 20/11/2015
 *
 ***********************************************/
#ifndef BINSON_UTF8_H_INCLUDED
#define BINSON_UTF8_H_INCLUDED

#include "binson_config.h"
#include "binson/binson_error.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  UTF-8 string helpers
 */
bool   binson_utf8_is_valid( uint8_t* string );
size_t binson_utf8_unescape( uint8_t *buf, size_t sz, uint8_t *src );

#ifdef __cplusplus
}
#endif

#endif /* BINSON_UTF8_H_INCLUDED */