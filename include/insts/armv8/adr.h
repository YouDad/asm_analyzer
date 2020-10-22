#pragma once
#include "types/instruction.h"
#include "utils/insts.h"

static inline int _translate_adr(const struct instruction *inst, char *str, int *str_cnt, int len)
{
	// xd, xn/imm
	char xd[8], xn[8];
	sscanf(inst->string, "%*s\t%[^,], %s", xd, xn);
	addr_printf("%s = %s;", xd, xn[0] == '#' ? &xn[1] : xn);
	return 0;
}
