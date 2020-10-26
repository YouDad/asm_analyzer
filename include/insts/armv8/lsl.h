#pragma once
#include "types/instruction.h"
#include "utils/insts.h"

static const char *_test_inst_lsl[] = {
	"lsl\tx0, x1, x2", 0,
	"lsl\tx0, x1, #3", 0,
};
static const char *_test_result_lsl[] = {
	"x0 = x1 << x2;", 0,
	"x0 = x1 << 3;", 0,
};

static inline int _translate_lsl(const struct instruction *inst, char *str, int *str_cnt, int len)
{
	// rd, rn, rm
	// rd, rn, #imm
	char rd[8], rn[8], rmimm[20], *rm = rmimm;
	int ret = sscanf(inst->string, "%*s\t"
			"%[^,], %[^,], %s", rd, rn, rmimm);
	if (rmimm[0] == '#') {
		rm = &rmimm[1];
	}
	if (ret != 3) {
		return 1;
	}

	addr_printf("%s = %s << %s;", rd, rn, rm);
	return 0;
}
