#pragma once
#include "types/instruction.h"
#include "utils/color.h"

static inline int _translate_bl(const struct instruction *inst, char *str, int *str_cnt, int len)
{
	char addr[16];
	int ret = sscanf(inst->string, "%*s\t" "%s", addr);
	if (ret != 1) {
		return 1;
	}

	*str_cnt += snprintf(&str[*str_cnt], len, "\t%x:\t", inst->addr);
	*str_cnt += snprintf(&str[*str_cnt], len, _green("text_0x%s();"), addr);
	return 0;
}
