#pragma once
#include "types/instruction.h"
#include "utils/color.h"

static inline int _translate_b_cond(const struct instruction *inst, char *str, int *str_cnt, int len)
{
	char op[4], addr[16];
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

	*str_cnt += snprintf(&str[*str_cnt], len, "\t%x:\t", inst->addr);
	*str_cnt += snprintf(&str[*str_cnt], len,
			_green("if %s then goto "), mean[i]);
	return 0;
}
