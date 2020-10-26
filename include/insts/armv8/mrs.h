#pragma once
#include "types/instruction.h"
#include "utils/insts.h"

static const char *_test_inst_mrs[] = {
	"mrs\tx8, sctlr_el2", 0,
};
static const char *_test_result_mrs[] = {
	"x8 = read_sysreg(sctlr_el2);", 0,
};

static inline int _translate_mrs(const struct instruction *inst, char *str, int *str_cnt, int len)
{
	// xt, sys
	char xd[20], xn[20];
	sscanf(inst->string, "%*s\t%[^,], %s", xd, xn);
	addr_printf("%s = read_sysreg(%s);", xd, xn[0] == '#' ? &xn[1] : xn);
	return 0;
}
