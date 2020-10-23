#pragma once
#include "types/instruction.h"
#include "utils/insts.h"

static const char *_test_inst_movk[] = {
	"movk\tw8, #0x30c5", 0,
	"movk\tw8, #0x30c5, lsl #16", 0,
};
static const char *_test_result_movk[] = {
	"w8 = 0x30c5;", 0,
	"w8 = (w8 & ~0xffff0000) | (0x30c5 << 16);", 0,
};

static inline int _translate_movk(const struct instruction *inst, char *str, int *str_cnt, int len)
{
	// rd, #imm{, lsl #amount}
	char xd[8], xn[20], other[20];
	int ret = sscanf(inst->string, "%*s\t%[^,], #%[0-9xa-f]%[^.]", xd, xn, other);
	if (ret == 2) {
		addr_printf("%s = %s;", xd, xn[0] == '#' ? &xn[1] : xn);
	} else {
		int shift;
		ret = sscanf(other, ", lsl #%d", &shift);
		if (ret == 1) {
			addr_printf("%s = (%s & ~0x%x) | (%s << %d);", xd, xd, 0xffff << shift, xn, shift);
			return 0;
		}
	}
	return 0;
}
