#pragma once
#include "types/instruction.h"
#include "utils/insts.h"
#include "types/bool.h"

static const char *_test_inst_str[] = {
	"str\tx0, [sp], #8", 0,
	"str\tx1, [sp, #-16]!", 0,
	"str\tx2, [sp, #48]", 0,
};
static const char *_test_result_str[] = {
	"(uint64_t *)sp[0] = x0;",
	"sp += 8;", 0,
	"sp += -16;",
	"(uint64_t *)sp[0] = x1;", 0,
	"(uint64_t *)sp[6] = x2;", 0,
};

static inline int _translate_str(const struct instruction *inst, char *str, int *str_cnt, int len)
{
	char rt[8], rn[8], other[20];
	int ret, imm;
	bool preindex, postindex;

	if (strstr(inst->string, "],") != NULL) {
		// rt, [rn], #imm
		ret = sscanf(inst->string, "%*s\t"
				"%[^,], [%[^]]], #%d", rt, rn, &imm);
		preindex = false;
		postindex = true;
	} else if (strstr(inst->string, "]!") != NULL) {
		// rt, [rn, #imm]!
		ret = sscanf(inst->string, "%*s\t"
				"%[^,], [%[^,], #%d]!", rt, rn, &imm);
		preindex = true;
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

		preindex = false;
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

	if (preindex) {
		addr_printf("%s += %d;\n", rn, imm);
		imm = 0;
	}

	if (!postindex) {
		addr_printf("(uint%d_t *)%s[%d] = %s;", datasize, rn, imm * 8 / datasize, rt);
	}

	if (postindex) {
		addr_printf("(uint%d_t *)%s[0] = %s;", datasize, rn, rt);
		addr_printf("%s += %d;", rn, imm);
	}
	return 0;
}
