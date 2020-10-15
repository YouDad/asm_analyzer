#include <stdio.h>
#include <string.h>
#include <errno.h>

#define array_size(x) (sizeof(x) / sizeof(typeof(x[0])))
