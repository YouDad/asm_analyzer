#pragma once
#include "types/instruction.h"
#include "utils/insts.h"

static const char *_test_inst_cbz[] = {
	"cbz\tw0, 123", 0,
};
static const char *_test_result_cbz[] = {
	"if w0 == 0 then goto", 0,
};

static inline int _translate_cbz(const struct instruction *inst, char *str, int *str_cnt, int len)
{
	char rt[8], addr[20];
	int ret = sscanf(inst->string, "%*s\t" "%[^,], %s", rt, addr);
	if (ret != 2) {
		return 1;
	}

	addr_printf("if %s == 0 then goto ", rt);
	return 0;
}
