#pragma once
#include "types/instruction.h"
#include "utils/color.h"

static inline int _translate_cbz(const struct instruction *inst, char *str, int *str_cnt, int len)
{
	char rt[4], addr[16];
	int ret = sscanf(inst->string, "%*s\t" "%[^,], %s", rt, addr);
	if (ret != 2) {
		return 1;
	}

	*str_cnt += snprintf(&str[*str_cnt], len, "\t%x:\t", inst->addr);
	*str_cnt += snprintf(&str[*str_cnt], len,
			_green("if %s == 0 then goto text_0x%s();"), rt, addr);
	return 0;
}
