#pragma once
#include "types/instruction.h"
#include "utils/insts.h"

static const char *_test_inst_mrs[] = {};
static const char *_test_result_mrs[] = {};

static inline int _translate_mrs(const struct instruction *inst, char *str, int *str_cnt, int len)
{
	// xd, xn/imm
	char xd[20], xn[20];
	sscanf(inst->string, "%*s\t%[^,], %s", xd, xn);
	addr_printf("%s = %s;", xd, xn[0] == '#' ? &xn[1] : xn);
	return 0;
}
