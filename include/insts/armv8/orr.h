#pragma once
#include "types/instruction.h"
#include "utils/color.h"

static inline int _translate_orr(const struct instruction *inst, char *str, int *str_cnt, int len)
{
	// rd, rn, imm
	// rd, rn, rm{, shift #amount}

	int ret;
	*str_cnt += snprintf(&str[*str_cnt], len, "\t%x:\t", inst->addr);
	if (strstr(inst->string, ", #") != NULL) {
		char rd[8], rn[8], imm[8];
		ret = sscanf(inst->string, "%*s\t"
				"%[^,], %[^,], #%[^,]", rd, rn, imm);
		if (ret == 3) {
			*str_cnt += snprintf(&str[*str_cnt], len,
					_green("%s = %s | %s;"), rd, rn, imm);
			return 0;
		} else {
			return 1;
		}
	} else {
		char rd[8], rn[8], rm[8], other[16];
		ret = sscanf(inst->string, "%*s\t"
				"%[^,], %[^,], %[^,]%[^.]",
				rd, rn, rm, other);
		if (ret == 3) {
			*str_cnt += snprintf(&str[*str_cnt], len,
					_green("%s = %s | %s;"), rd, rn, rm);
			return 0;
		}

		char shift[8], amount[8];
		ret = sscanf(other, ", %s #%s", shift, amount);
		if (ret != 2) {
			return 1;
		}

		if (strcmp(shift, "lsl") == 0) {
			*str_cnt += snprintf(&str[*str_cnt], len,
					_green("%s = %s | (%s << %s);"), rd, rn, rm, amount);
		} else if (strcmp(shift, "lsr") == 0) {
			*str_cnt += snprintf(&str[*str_cnt], len,
					_green("%s = %s | (%s >> %s);"), rd, rn, rm, amount);
		} else if (strcmp(shift, "asr") == 0) {
			*str_cnt += snprintf(&str[*str_cnt], len,
					_green("%s = %s | (%s / (1 << %s));"), rd, rn, rm, amount);
		} else if (strcmp(shift, "ror") == 0) {
			int _amount;
			sscanf(amount, "%d", &_amount);
			*str_cnt += snprintf(&str[*str_cnt], len,
					_green("%s = %s | ((%s >> %d) | ((%s & %x) << %d));"),
					rd, rn, rm, _amount, rm, (1 << _amount) - 1, 64 - _amount);
		} else {
			return 1;
		}
		return 0;
	}
}
