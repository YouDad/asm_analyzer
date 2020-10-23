#pragma once
#include "types/instruction.h"
#include "utils/insts.h"
#include "types/bool.h"

static const char *_test_inst_stp[] = {
	"stp\tx0, x1, [x6], #32", 0,
	"stp\tx2, x3, [sp, #-96]!", 0,
	"stp\tx4, x5, [sp, #48]", 0,
};
static const char *_test_result_stp[] = {
	"(uint64_t *)x6[0] = x0;",
	"(uint64_t *)x6[1] = x1;",
	"x6 += 32;", 0,
	"sp += -96;",
	"(uint64_t *)sp[0] = x2;",
	"(uint64_t *)sp[1] = x3;", 0,
	"(uint64_t *)sp[6] = x4;",
	"(uint64_t *)sp[7] = x5;", 0,
};

static inline int _translate_stp(const struct instruction *inst, char *str, int *str_cnt, int len)
{
	char rt1[8], rt2[8], rn[8], other[20];
	int ret, imm;
	bool preindex, postindex;

	if (strstr(inst->string, "],") != NULL) {
		// rt1, rt2, [rn], #imm
		ret = sscanf(inst->string, "%*s\t"
				"%[^,], %[^,], [%[^]]], #%d", rt1, rt2, rn, &imm);
		preindex = false;
		postindex = true;
	} else if (strstr(inst->string, "]!") != NULL) {
		// rt1, rt2, [rn, #imm]!
		ret = sscanf(inst->string, "%*s\t"
				"%[^,], %[^,], [%[^,], #%d]!", rt1, rt2, rn, &imm);
		preindex = true;
		postindex = false;
	} else {
		// rt1, rt2, [rn{, #imm}]
		ret = sscanf(inst->string, "%*s\t"
				"%[^,], %[^,], [%[^],]%[^.]", rt1, rt2, rn, other);
		if (other[0] == ',') {
			sscanf(other, ", #%d", &imm);
		} else {
			imm = 0;
		}

		preindex = false;
		postindex = false;
	}

	if (ret != 4) {
		return 1;
	}

	int datasize = rt1[0] == 'x' ? 64 : 32;

	if (preindex) {
		addr_printf("%s += %d;\n", rn, imm);
		imm = 0;
	}

	if (!postindex) {
		addr_printf("(uint%d_t *)%s[%d] = %s;\n", datasize, rn, imm * 8 / datasize, rt1);
		addr_printf("(uint%d_t *)%s[%d] = %s;", datasize, rn, imm * 8 / datasize + 1, rt2);
	}

	if (postindex) {
		addr_printf("(uint%d_t *)%s[0] = %s;\n", datasize, rn, rt1);
		addr_printf("(uint%d_t *)%s[1] = %s;\n", datasize, rn, rt2);
		addr_printf("%s += %d;", rn, imm);
	}
	return 0;
}
