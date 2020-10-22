#pragma once
#include "types/instruction.h"
#include "utils/insts.h"

static inline int _translate_cbnz(const struct instruction *inst, char *str, int *str_cnt, int len)
{
	char rt[4], addr[16];
	int ret = sscanf(inst->string, "%*s\t" "%[^,], %s", rt, addr);
	if (ret != 2) {
		return 1;
	}

	addr_printf("if %s != 0 then goto ", rt);
	return 0;
}
