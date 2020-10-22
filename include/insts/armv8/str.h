#pragma once
#include "types/instruction.h"
#include "utils/color.h"
#include "types/bool.h"

static inline int _translate_str(const struct instruction *inst, char *str, int *str_cnt, int len)
{
	char rt[8], rn[8], other[16];
	int ret, imm;
	bool wback, postindex;

	*str_cnt += snprintf(&str[*str_cnt], len, "\t%x:\t", inst->addr);
	if (strstr(inst->string, "],") != NULL) {
		// rt, [rn], #imm
		ret = sscanf(inst->string, "%*s\t"
				"%[^,], [%[^]]], #%d", rt, rn, &imm);
		wback = true;
		postindex = true;
	} else if (strstr(inst->string, "]!") != NULL) {
		// rt, [rn, #imm]!
		ret = sscanf(inst->string, "%*s\t"
				"%[^,], [%[^,], #%d]!", rt, rn, &imm);
		wback = true;
		postindex = false;
	} else {
		// rt, [rn{, #imm}]
		ret = sscanf(inst->string, "%*s\t"
				"%[^,], [%[^],]%[^.]", rt, rn, other);
		if (other[0] == ',') {
			sscanf(other, ", #%d", &imm);
		} else {
			imm = 0;
		}

		wback = false;
		postindex = false;
	}

	if (ret != 3) {
		printf("%s\n%d: %s, %s, %d\n", inst->string, ret, rt, rn, imm);
		return 1;
	}

	int datasize = 32;
	if (rt[0] == 'x') {
		datasize = 64;
	}

	bool first = true;
	if (wback && !postindex) {
		if (!first) {
			*str_cnt += snprintf(&str[*str_cnt], len, "\n\t%x:\t", inst->addr);
		}
		first = false;
		*str_cnt += snprintf(&str[*str_cnt], len,
				_green("%s += %d;"), rn, imm);
		imm = 0;
	}
	if (!first) {
		*str_cnt += snprintf(&str[*str_cnt], len, "\n\t%x:\t", inst->addr);
	}
	first = false;
	*str_cnt += snprintf(&str[*str_cnt], len,
			_green("(uint%d_t *)%s[%d] = %s;"),
			datasize, rn, imm * 8 / datasize, rt);
	if (wback && postindex) {
		if (!first) {
			*str_cnt += snprintf(&str[*str_cnt], len, "\n\t%x:\t", inst->addr);
		}
		first = false;
		*str_cnt += snprintf(&str[*str_cnt], len,
				_green("%s += %d;"), rn, imm);
	}
	return 0;
}
