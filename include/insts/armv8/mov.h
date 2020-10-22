#pragma once
#include "types/instruction.h"
#include "utils/insts.h"

static const char *_test_inst_mov[] = {
	"mov\tw0, #0x1", 0,
};
static const char *_test_result_mov[] = {
	"w0 = 0x1;", 0,
};

static inline int _translate_mov(const struct instruction *inst, char *str, int *str_cnt, int len)
{
	// xd, xn/imm
	char xd[8], xn[20];
	sscanf(inst->string, "%*s\t%[^,], %s", xd, xn);
	addr_printf("%s = %s;", xd, xn[0] == '#' ? &xn[1] : xn);
	return 0;
}
