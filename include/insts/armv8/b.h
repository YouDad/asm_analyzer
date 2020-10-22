#pragma once
#include "types/instruction.h"
#include "utils/color.h"

static inline int _translate_b(const struct instruction *inst, char *str, int *str_cnt, int len)
{
	*str_cnt += snprintf(&str[*str_cnt], len, "\t%x:\t", inst->addr);
	*str_cnt += snprintf(&str[*str_cnt], len, _green("goto "));
	return 0;
}
