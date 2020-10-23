#pragma once
#include "types/instruction.h"
#include "utils/insts.h"

static const char *_test_inst_orr[] = {
	"orr\tx5, x1, #0x3", 0,
	"orr\tx5, x1, x0", 0,
	"orr\tx5, x1, x0, lsl #12", 0,
	"orr\tx5, x1, x0, lsr #12", 0,
	"orr\tx5, x1, x0, asr #12", 0,
	"orr\tx5, x1, x0, ror #12", 0,
};
static const char *_test_result_orr[] = {
	"x5 = x1 | 0x3;", 0,
	"x5 = x1 | x0;", 0,
	"x5 = x1 | (x0 << 12);", 0,
	"x5 = x1 | (x0 >> 12);", 0,
	"x5 = x1 | (x0 / (1 << 12));", 0,
	"x5 = x1 | ((x0 >> 12) | ((x0 & 0xfff) << 52));", 0,
};

static inline int _translate_orr(const struct instruction *inst, char *str, int *str_cnt, int len)
{
	// rd, rn, #imm
	// rd, rn, rm{, lsl/lsr/asr/ror #amount}

	int ret;
	if (strstr(inst->string, ", #") != NULL) {
		char rd[8], rn[8], imm[20];
		ret = sscanf(inst->string, "%*s\t"
				"%[^,], %[^,], #%[^,]", rd, rn, imm);
		if (ret == 3) {
			addr_printf("%s = %s | %s;", rd, rn, imm);
			return 0;
		} else {
			return 1;
		}
	} else {
		char rd[8], rn[8], rm[8], other[20];
		ret = sscanf(inst->string, "%*s\t"
				"%[^,], %[^,], %[^,]%[^.]",
				rd, rn, rm, other);
		if (ret == 3) {
			addr_printf("%s = %s | %s;", rd, rn, rm);
			return 0;
		}

		char shift[8], amount[8];
		ret = sscanf(other, ", %s #%s", shift, amount);
		if (ret != 2) {
			return 1;
		}

		if (strcmp(shift, "lsl") == 0) {
			addr_printf("%s = %s | (%s << %s);", rd, rn, rm, amount);
		} else if (strcmp(shift, "lsr") == 0) {
			addr_printf("%s = %s | (%s >> %s);", rd, rn, rm, amount);
		} else if (strcmp(shift, "asr") == 0) {
			addr_printf("%s = %s | (%s / (1 << %s));", rd, rn, rm, amount);
		} else if (strcmp(shift, "ror") == 0) {
			int _amount;
			sscanf(amount, "%d", &_amount);
			addr_printf("%s = %s | ((%s >> %d) | ((%s & 0x%x) << %d));", rd, rn, rm, _amount, rm, (1 << _amount) - 1, 64 - _amount);
		} else {
			return 1;
		}
		return 0;
	}
}
