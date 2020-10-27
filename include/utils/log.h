#pragma once
#include "utils/color.h"
#include <stdlib.h>

#define log()                printf(_green("(%s: %d)") "\n", __FILE__, __LINE__)
#define info(fmt, ...)       printf("(%s: %d)" _cyan(fmt) "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#define warn(fmt, ...)       printf("(%s: %d)" _yellow(fmt) "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#define error(ret, fmt, ...) printf("(%s: %d)" _red(fmt)  "\n", __FILE__, __LINE__, ##__VA_ARGS__); return (ret)
#define fatal(ret, fmt, ...) printf("(%s: %d)" _red(fmt)  "\n", __FILE__, __LINE__, ##__VA_ARGS__); exit(ret)
