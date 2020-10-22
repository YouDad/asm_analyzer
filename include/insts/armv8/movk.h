#pragma once
#include "types/instruction.h"
#include "utils/insts.h"

static inline int _translate_movk(const struct instruction *inst, char *str, int *str_cnt, int len)
{
	// rd, #imm{, lsl #shift}
	char xd[8], xn[8], other[16];
	int ret = sscanf(inst->string, "%*s\t%[^,], #%[0-9xa-f]%[^.]", xd, xn, other);
	if (ret == 2) {
		addr_printf("%s = %s;", xd, xn[0] == '#' ? &xn[1] : xn);
	} else {
		int shift;
		ret = sscanf(other, ", lsl #%d", &shift);
		if (ret == 1) {
			addr_printf("%s = (%s & 0x%x) | (%s << %d);", xd, xd, 0xffff << shift, xn, shift);
			return 0;
		}
	}
	return 0;
}
