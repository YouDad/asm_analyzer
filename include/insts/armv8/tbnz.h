#pragma once
#include "types/instruction.h"
#include "utils/insts.h"

static const char *_test_inst_tbnz[] = {
	"tbnz\tw0, #3, 456", 0,
};
static const char *_test_result_tbnz[] = {
	"if (w0 & (1 << 3)) goto ", 0,
};

static inline int _translate_tbnz(const struct instruction *inst, char *str, int *str_cnt, int len)
{
	// rt, #imm, label
	char rt[8], imm[20];
	int ret = sscanf(inst->string, "%*s\t"
			"%[^,], #%[^,],", rt, imm);
	if (ret != 2) {
		return 1;
	}

	addr_printf("if (%s & (1 << %s)) goto ", rt, imm);
	return 0;
}
