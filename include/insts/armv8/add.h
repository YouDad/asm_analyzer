#pragma once
#include "types/instruction.h"
#include "utils/color.h"

static inline int _translate_add(const struct instruction *inst, char *str, int *str_cnt, int len)
{
	// rd, rn, rm{, extend {#amount}}
	// rd, rn, imm{, shift}
	// rd, rn, rm{, shift}
	char rd[8], rn[8], rmimm[8], other[16];
	int ret = sscanf(inst->string, "%*s\t"
			"%[^,], %[^,], %[^,]%[^.]",
			rd, rn, rmimm, other);
	if (ret == 3) {
		*str_cnt += snprintf(&str[*str_cnt], len, "\t%x:\t", inst->addr);
		*str_cnt += snprintf(&str[*str_cnt], len,
				_green("%s = %s + %s;"),
				rd, rn, rmimm[0] == '#' ? &rmimm[1] : rmimm);
		return 0;
	} else {
		return 1;
	}

	int shift = 0;
	char extend[8];
	if (strstr(other, "#") != NULL) {
		ret = sscanf(other, ", %s #%d", extend, &shift);
		if (ret != 2) {
			return 1;
		}
	} else {
		sscanf(other, ", %s", extend);
		if (ret != 1) {
			return 1;
		}
	}

	if (strcmp(extend, "lsl") == 0) {
		if (rd[0] == 'x') {
			strcpy(extend, "uxtx");
		} else {
			strcpy(extend, "uxtw");
		}
	}

	char types[8];
	switch (extend[3]) {
		case 'b': strcpy(types, "int8_t"); break;
		case 'h': strcpy(types, "int16_t"); break;
		case 'w': strcpy(types, "int32_t"); break;
		case 'x': strcpy(types, "int64_t"); break;
	}

	if (shift) {
		*str_cnt += snprintf(&str[*str_cnt], len, "\t%x:\t", inst->addr);
		*str_cnt += snprintf(&str[*str_cnt], len,
				_green("%s = %s + (%s%s)(%s << %d);"),
				rd, rn, extend[0] == 'u' ? "u" : "", types, rmimm, shift);
	} else {
		*str_cnt += snprintf(&str[*str_cnt], len, "\t%x:\t", inst->addr);
		*str_cnt += snprintf(&str[*str_cnt], len,
				_green("%s = %s + (%s%s)%s;"),
				rd, rn, extend[0] == 'u' ? "u" : "", types, rmimm);
	}
	return 0;
}