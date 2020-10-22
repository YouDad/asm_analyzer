#pragma once
#include "types/instruction.h"
#include "utils/insts.h"

static inline int _translate_bl(const struct instruction *inst, char *str, int *str_cnt, int len)
{
	char addr[16];
	int ret = sscanf(inst->string, "%*s\t" "%s", addr);
	if (ret != 1) {
		return 1;
	}

	addr_printf("text_0x%s();", addr);
	return 0;
}
