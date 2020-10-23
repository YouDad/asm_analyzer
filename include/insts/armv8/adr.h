#pragma once
#include "types/instruction.h"
#include "utils/insts.h"

static const char *_test_inst_adr[] = {
	"adr\tx0, 123", 0,
};
static const char *_test_result_adr[] = {
	"x0 = 0x123;", 0,
};

static inline int _translate_adr(const struct instruction *inst, char *str, int *str_cnt, int len)
{
	// xd, label
	char xd[8], label[20];
	sscanf(inst->string, "%*s\t%[^,], %s", xd, label);
	addr_printf("%s = 0x%s;", xd, label);
	return 0;
}
