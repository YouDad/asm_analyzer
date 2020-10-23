#pragma once
#include "types/instruction.h"
#include "utils/insts.h"

static const char *_test_inst_sub[] = {
	"sub\tx0, x1, x2", 0,
	"sub\tx0, x1, x2, uxtw #1", 0,
	"sub\tx0, x1, x2, sxtx", 0,
	"sub\tx9, x30, sp, lsl #32", 0,
};
static const char *_test_result_sub[] = {
	"x0 = x1 - x2;", 0,
	"x0 = x1 - (uint32_t)(x2 << 1);", 0,
	"x0 = x1 - (int64_t)x2;", 0,
	"x9 = x30 - (uint64_t)(sp << 32);", 0,
};

static inline int _translate_sub(const struct instruction *inst, char *str, int *str_cnt, int len)
{
	// rd, rn, #imm{, lsl #0/#12}
	// rd, rn, rm{, lsl/lsr/asr #amount}
	// rd, rn, rm{, extend{ #amount}}
	char rd[8], rn[8], rmimm[20], other[20];
	int ret = sscanf(inst->string, "%*s\t"
			"%[^,], %[^,], %[^,]%[^.]", rd, rn, rmimm, other);
	if (ret == 3) {
		addr_printf("%s = %s - %s;",
				rd, rn, rmimm[0] == '#' ? &rmimm[1] : rmimm);
		return 0;
	}

	int shift = 0;
	char extend[8];
	if (strstr(other, "#") != NULL) {
		ret = sscanf(other, ", %s #%d", extend, &shift);
		if (ret != 2) {
			return 1;
		}
	} else {
		ret = sscanf(other, ", %s", extend);
		if (ret != 1) {
			return 1;
		}
	}

	if (strcmp(extend, "lsl") == 0) {
		if (rd[0] == 'x') {
			strcpy(extend, "uxtx");
		} else {
			strcpy(extend, "uxtw");
		}
	}

	char types[8];
	switch (extend[3]) {
		case 'b': strcpy(types, "int8_t"); break;
		case 'h': strcpy(types, "int16_t"); break;
		case 'w': strcpy(types, "int32_t"); break;
		case 'x': strcpy(types, "int64_t"); break;
	}

	if (shift) {
		addr_printf("%s = %s - (%s%s)(%s << %d);",
				rd, rn, extend[0] == 'u' ? "u" : "", types, rmimm, shift);
	} else {
		addr_printf("%s = %s - (%s%s)%s;",
				rd, rn, extend[0] == 'u' ? "u" : "", types, rmimm);
	}
	return 0;
}
