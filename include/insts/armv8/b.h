#pragma once
#include "types/instruction.h"
#include "utils/insts.h"

static const char *_test_inst_b[] = {
	"b\t123", 0,
};
static const char *_test_result_b[] = {
	"goto ", 0,
};

static inline int _translate_b(const struct instruction *inst, char *str, int *str_cnt, int len)
{
	addr_printf("goto ");
	return 0;
}
