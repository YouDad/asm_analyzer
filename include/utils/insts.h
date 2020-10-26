#pragma once
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include "types/bool.h"
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

static inline const char *ext(const char *extend, bool bit_64)
{
	switch(extend[0]) {
		case 'u':
			if (extend[1] == 'x' && extend[2] == 't') {
				switch(extend[3]) {
					case 'b':
						return "uint8_t";
					case 'h':
						return "uint16_t";
					case 'w':
						return "uint32_t";
					case 'x':
						return "uint64_t";
				}
			}
			break;
		case 's':
			if (extend[1] == 'x' && extend[2] == 't') {
				switch(extend[3]) {
					case 'b':
						return "int8_t";
					case 'h':
						return "int16_t";
					case 'w':
						return "int32_t";
					case 'x':
						return "int64_t";
				}
			}
			break;
		case 'l':
			if (extend[1] == 's' && extend[2] == 'l') {
				return bit_64 ? "uint64_t" : "uint32_t";
			}
			break;
	}

	return "unknown";
}

void set_cmp(uint32_t addr, char *r1, char *r2);
bool get_cmp(uint32_t *addr, char r1[], char r2[]);
