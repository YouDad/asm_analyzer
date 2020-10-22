#pragma once
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include "utils/color.h"

static inline void str_printf(char *str, int *str_cnt, int len,
		const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);

	*str_cnt += vsnprintf(&str[*str_cnt], len, fmt, ap);

	va_end(ap);
}

#define addr_printf(fmt, ...) \
	str_printf(str, str_cnt, len, "\t%x:\t" _green(fmt), inst->addr, ##__VA_ARGS__)

#define addr_str_printf(str, str_cnt, len, addr, fmt, ...) \
	str_printf(str, str_cnt, len, "\t%x:\t" _green(fmt), addr, ##__VA_ARGS__)
