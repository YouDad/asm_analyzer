#pragma once
#include "types/instruction.h"
#include "utils/insts.h"
#include "types/bool.h"

static inline int _translate_ldp(const struct instruction *inst, char *str, int *str_cnt, int len)
{
	char rt1[8], rt2[8], rn[8], other[16];
	int ret, imm;
	bool wback, postindex;

	if (strstr(inst->string, "],") != NULL) {
		// rt1, rt2, [rn], #imm
		ret = sscanf(inst->string, "%*s\t"
				"%[^,], %[^,], [%[^]]], #%d", rt1, rt2, rn, &imm);
		wback = true;
		postindex = true;
	} else if (strstr(inst->string, "]!") != NULL) {
		// rt1, rt2, [rn, #imm]!
		ret = sscanf(inst->string, "%*s\t"
				"%[^,], %[^,], [%[^,], #%d]!", rt1, rt2, rn, &imm);
		wback = true;
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

		wback = false;
		postindex = false;
	}

	if (ret != 4) {
		printf("%s\n%d: %s, %s, %s, %d\n", inst->string, ret, rt1, rt2, rn, imm);
		return 1;
	}

	int datasize = 32;
	if (rt1[0] == 'x') {
		datasize = 64;
	}

	bool first = true;
	if (wback && !postindex) {
		addr_printf("%s += %d;", rn, imm);
		imm = 0;
	}
	addr_printf("%s = (uint%d_t *)%s[%d];", rt1, datasize, rn, imm * 8 / datasize);
	addr_printf("%s = (uint%d_t *)%s[%d];", rt2, datasize, rn, imm * 8 / datasize + 1);
	if (wback && postindex) {
		addr_printf("%s += %d;", rn, imm);
	}
	return 0;
}
