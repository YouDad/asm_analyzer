#pragma once
#include "types/instruction.h"
#include "utils/color.h"

static inline int _translate_movk(const struct instruction *inst, char *str, int *str_cnt, int len)
{
	// rd, #imm{, lsl #shift}
	char xd[8], xn[8], other[16];
	int ret = sscanf(inst->string, "%*s\t%[^,], #%[0-9xa-f]%[^.]", xd, xn, other);
	*str_cnt += snprintf(&str[*str_cnt], len, "\t%x:\t", inst->addr);
	if (ret == 2) {
		*str_cnt += snprintf(&str[*str_cnt], len,
				_green("%s = %s;"), xd, xn[0] == '#' ? &xn[1] : xn);
	} else {
		int shift;
		ret = sscanf(other, ", lsl #%d", &shift);
		if (ret == 1) {
			*str_cnt += snprintf(&str[*str_cnt], len,
					_green("%s = (%s & 0x%x) | (%s << %d);"),
					xd, xd, 0xffff << shift, xn, shift);
			return 0;
		}
	}
	return 0;
}
