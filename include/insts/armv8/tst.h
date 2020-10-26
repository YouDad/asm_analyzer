#pragma once
#include "types/instruction.h"
#include "utils/insts.h"

static const char *_test_inst_tst[] = {
};
static const char *_test_result_tst[] = {
};

static inline int _translate_tst(const struct instruction *inst, char *str, int *str_cnt, int len)
{
	char r[20], rimm[20] = "&";
	int ret = sscanf(inst->string, "%*s\t" "%[^,], %s", r, &rimm[1]);
	if (ret != 2) {
		return -1;
	}

	set_cmp(inst->addr, r, rimm);
	return 1;
}
