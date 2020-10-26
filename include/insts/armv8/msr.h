#pragma once
#include "types/instruction.h"
#include "utils/insts.h"

static const char *_test_inst_msr[] = {
	"msr\tsctlr_el2, x8", 0,
};
static const char *_test_result_msr[] = {
	"write_sysreg(sctlr_el2, x8);", 0,
};

static inline int _translate_msr(const struct instruction *inst, char *str, int *str_cnt, int len)
{
	// sys, xt
	// sys, #imm
	char xd[20], xn[20];
	sscanf(inst->string, "%*s\t%[^,], %s", xd, xn);
	addr_printf("write_sysreg(%s, %s);", xd, xn[0] == '#' ? &xn[1] : xn);
	return 0;
}
