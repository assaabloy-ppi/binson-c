/**
 *  General BINSON lib features like version checking, etc
 */

#include <stdio.h>
#include "binson/binson.h"

int main()
{
  printf("binson headers version:\t\t %d.%d.%d\n", BINSON_MAJOR_VERSION, BINSON_MINOR_VERSION, BINSON_MICRO_VERSION);
  printf("libbinson.so version:\t\t %d.%d.%d\n", binson_lib_get_version() >> 16 & 0xff,
						 binson_lib_get_version() >> 8 & 0xff,
						 binson_lib_get_version() & 0xff );
  
  return 0;
}
