#pragma once
#include "types/instruction.h"
#include "utils/insts.h"

static inline int _translate_b_cond(const struct instruction *inst, char *str, int *str_cnt, int len)
{
	char op[8], addr[20];
	int ret = sscanf(inst->string, "b.%s\t" "%s", op, addr);
	if (ret != 2) {
		return 1;
	}

	const char *cond[] = {"eq", "ne", "cs", "cc", "mi",
		"pl", "vs", "vc", "hi", "ls", "ge", "lt", "gt", "le"};
	const char *mean[] = {"==", "!=", "u>=", "u<", "<0", ">=0",
		"overflow", "not overflow", "u>", "u<=", "s>=", "s<", "s>", "s<=", "unknown"};

	int i = 0;
	for (i = 0; i < array_size(cond); i++) {
		if (strcmp(op, cond[i]) == 0) {
			break;
		}
	}

	addr_printf("if %s then goto ", mean[i]);
	return 0;
}
