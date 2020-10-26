#pragma once
#include "types/instruction.h"
#include "utils/insts.h"
#include "types/bool.h"

static const char *_test_inst_ldp[] = {
	"ldp\tx0, x1, [x6], #32", 0,
	"ldp\tx2, x3, [sp, #-96]!", 0,
	"ldp\tx4, x5, [sp, #48]", 0,
};
static const char *_test_result_ldp[] = {
	"x0 = ((uint64_t *)x6)[0];",
	"x1 = ((uint64_t *)x6)[1];",
	"x6 += 32;", 0,
	"sp += -96;",
	"x2 = ((uint64_t *)sp)[0];",
	"x3 = ((uint64_t *)sp)[1];", 0,
	"x4 = ((uint64_t *)sp)[6];",
	"x5 = ((uint64_t *)sp)[7];", 0,
};

static inline int _translate_ldp(const struct instruction *inst, char *str, int *str_cnt, int len)
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
		printf("%s\n%d: %s, %s, %s, %d\n", inst->string, ret, rt1, rt2, rn, imm);
		return -1;
	}

	int datasize = 32;
	if (rt1[0] == 'x') {
		datasize = 64;
	}

	if (preindex) {
		addr_printf("%s += %d;\n", rn, imm);
		imm = 0;
	}

	if (!postindex) {
		addr_printf("%s = ((uint%d_t *)%s)[%d];\n", rt1, datasize, rn, imm * 8 / datasize);
		addr_printf("%s = ((uint%d_t *)%s)[%d];", rt2, datasize, rn, imm * 8 / datasize + 1);
	}

	if (postindex) {
		addr_printf("%s = ((uint%d_t *)%s)[0];\n", rt1, datasize, rn);
		addr_printf("%s = ((uint%d_t *)%s)[1];\n", rt2, datasize, rn);
		addr_printf("%s += %d;", rn, imm);
	}
	return 0;
}
