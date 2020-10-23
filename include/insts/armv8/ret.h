#pragma once
#include "types/instruction.h"
#include "utils/insts.h"
#include "types/bool.h"

static const char *_test_inst_ret[] = {
	"ret", 0,
	"ret x3", 0,
};
static const char *_test_result_ret[] = {
	"return;", 0,
	"return x3;", 0,
};

static inline int _translate_ret(const struct instruction *inst, char *str, int *str_cnt, int len)
{
	// {rt}
	char rt[8];
	int ret = sscanf(inst->string, "%*s\t%s", rt);
	if (ret != 1) {
		addr_printf("return;");
	} else {
		addr_printf("return %s;", rt);
	}
	return 0;
}
