#pragma once
#include "types/instruction.h"
#include "utils/insts.h"

static inline int _translate_adrp(const struct instruction *inst, char *str, int *str_cnt, int len)
{
	// xd, xn/imm
	char xd[8], xn[20];
	sscanf(inst->string, "%*s\t%[^,], %s", xd, xn);
	addr_printf("%s = 0x%s;", xd, xn);
	return 0;
}
