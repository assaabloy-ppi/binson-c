#ifndef _BTEST_H
#define _BTEST_H

/*#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>

#include <cmocka.h>

#include "binson/binson.h"

/*
 * This function must be defined in every unit test file.
 */
int utest_run_tests(void);

#endif /* _BTEST_H */