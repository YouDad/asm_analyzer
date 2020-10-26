#pragma once
#include "types/instruction.h"
#include "utils/insts.h"

static const char *_test_inst_and[] = {
	"and\tx0, x0, #1", 0,
	"and\tx9, x30, #0xfffffffffffffff0, lsl #32", 0,
	"and\tx0, x1, x2", 0,
	"and\tx9, x30, sp, lsl #32", 0,
	"and\tw9, x30, sp, lsl #16", 0,
	"and\tw9, x30, sp, lsr #16", 0,
	"and\tw9, x30, sp, asr #16", 0,
	"and\tw9, x30, sp, ror #16", 0,
};
static const char *_test_result_and[] = {
	"x0 = x0 & 1;", 0,
	"x9 = x30 & (0xfffffffffffffff0 << 32)", 0,
	"x0 = x1 & x2;", 0,
	"x9 = x30 & (sp << 32);", 0,
	"w9 = x30 & (sp << 16);", 0,
	"w9 = x30 & (sp >> 16);", 0,
	"w9 = x30 & sp / (1 << 16);", 0,
	"w9 = x30 & ((sp >> 16) | ((sp & 0xffff) << 48));", 0,
};

static inline int _translate_and(const struct instruction *inst, char *str, int *str_cnt, int len)
{
	// rd, rn, #imm{, lsl #0/#12}
	// rd, rn, rm{, lsl/lsr/asr #amount}
	// rd, rn, rm{, extend{ #amount}}
	char rd[8], rn[8], rmimm[20], other[20], *rm = rmimm;
	int ret = sscanf(inst->string, "%*s\t"
			"%[^,], %[^,], %[^,]%[^.]",
			rd, rn, rmimm, other);
	if (rmimm[0] == '#') {
		rm = &rmimm[1];
	}
	if (ret == 3) {
		addr_printf("%s = %s & %s;", rd, rn, rm);
		return 0;
	}

	int shift = 0;
	char extend[8];
	if (strstr(other, "#") != NULL) {
		ret = sscanf(other, ", %s #%d", extend, &shift);
		if (ret != 2) {
			return -1;
		}
	} else {
		ret = sscanf(other, ", %s", extend);
		if (ret != 1) {
			return -1;
		}
	}

	bool is_shift = false;
	is_shift += 1 * (strcmp(extend, "lsl") == 0);
	is_shift += 2 * (strcmp(extend, "lsr") == 0);
	is_shift += 3 * (strcmp(extend, "asr") == 0);
	is_shift += 4 * (strcmp(extend, "ror") == 0);
	switch(is_shift) {
		case 1:
			addr_printf("%s = %s & (%s << %d);", rd, rn, rm, shift);
			return 0;
		case 2:
			addr_printf("%s = %s & (%s >> %d);", rd, rn, rm, shift);
			return 0;
		case 3:
			addr_printf("%s = %s & %s / (1 << %d);", rd, rn, rm, shift);
			return 0;
		case 4:
			addr_printf("%s = %s & ((%s >> %d) | ((%s & 0x%x) << %d));",
					rd, rn, rm, shift, rm, (1 << shift) - 1, 64 - shift);
			return 0;
		default:
			break;
	}

	return 0;
}
