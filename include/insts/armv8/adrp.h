#pragma once
#include "types/instruction.h"
#include "utils/insts.h"

static const char *_test_inst_adrp[] = {
	"adrp\tx0, c000", 0,
};
static const char *_test_result_adrp[] = {
	"x0 = 0xc000;", 0,
};

static inline int _translate_adrp(const struct instruction *inst, char *str, int *str_cnt, int len)
{
	// xd, label
	char xd[8], label[20];
	sscanf(inst->string, "%*s\t%[^,], %s", xd, label);
	addr_printf("%s = 0x%s;", xd, label);
	return 0;
}
