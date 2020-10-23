#pragma once
#include "types/instruction.h"
#include "utils/insts.h"

static const char *_test_inst_cbnz[] = {
	"cbnz\tw0, 123", 0,
};
static const char *_test_result_cbnz[] = {
	"if w0 != 0 then goto", 0,
};

static inline int _translate_cbnz(const struct instruction *inst, char *str, int *str_cnt, int len)
{
	// rt, label
	char rt[8], label[20];
	int ret = sscanf(inst->string, "%*s\t" "%[^,], %s", rt, label);
	if (ret != 2) {
		return 1;
	}

	addr_printf("if %s != 0 then goto ", rt);
	return 0;
}
