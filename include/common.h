#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <malloc.h>
#include <stdint.h>

#define array_size(x) (sizeof(x) / sizeof(typeof(x[0])))
#define MALLOC(type, number) (type *)malloc((number) * sizeof(type))
#define REALLOC(ptr, type, number) (type *)realloc((ptr), (number) * sizeof(type))

#define EINTERNAL (0x1000 + 0)
#define ENOADDR   (0x1000 + 1)
