#pragma once
#include "types/instruction.h"
#include "utils/insts.h"

static const char *_test_inst_add[] = {
	"add\tx0, x0, #1", 0,
	"add\tx0, x1, #0xfffffffffffffff0, uxtx #1", 0,
	"add\tx9, x30, #0xfffffffffffffff0, lsl #32", 0,
	"add\tx0, x1, x2", 0,
	"add\tx0, x1, x2, sxtx", 0,
	"add\tx0, x1, x2, uxtw #1", 0,
	"add\tx9, x30, sp, lsl #32", 0,
	"add\tw9, x30, sp, lsl #16", 0,
	"add\tw9, x30, sp, lsr #16", 0,
	"add\tw9, x30, sp, asr #16", 0,
};
static const char *_test_result_add[] = {
	"x0 = x0 + 1;", 0,
	"x0 = x1 + (uint64_t)(0xfffffffffffffff0 << 1);", 0,
	"x9 = x30 + (0xfffffffffffffff0 << 32)", 0,
	"x0 = x1 + x2;", 0,
	"x0 = x1 + (int64_t)x2;", 0,
	"x0 = x1 + (uint32_t)(x2 << 1);", 0,
	"x9 = x30 + (sp << 32);", 0,
	"w9 = x30 + (sp << 16);", 0,
	"w9 = x30 + (sp >> 16);", 0,
	"w9 = x30 + sp / (1 << 16);", 0,
};

static inline int _translate_add(const struct instruction *inst, char *str, int *str_cnt, int len)
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
		addr_printf("%s = %s + %s;", rd, rn, rm);
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
	switch(is_shift) {
		case 1:
			addr_printf("%s = %s + (%s << %d);", rd, rn, rm, shift);
			return 0;
		case 2:
			addr_printf("%s = %s + (%s >> %d);", rd, rn, rm, shift);
			return 0;
		case 3:
			addr_printf("%s = %s + %s / (1 << %d);", rd, rn, rm, shift);
			return 0;
		default:
			break;
	}

	char types[8] = "unknown";
	switch (extend[3]) {
		case 'b': strcpy(types, "int8_t"); break;
		case 'h': strcpy(types, "int16_t"); break;
		case 'w': strcpy(types, "int32_t"); break;
		case 'x': strcpy(types, "int64_t"); break;
	}

	if (shift) {
		addr_printf("%s = %s + (%s%s)(%s << %d);",
				rd, rn, extend[0] == 'u' ? "u" : "", types, rm, shift);
	} else {
		addr_printf("%s = %s + (%s%s)%s;",
				rd, rn, extend[0] == 'u' ? "u" : "", types, rm);
	}

	return 0;
}
