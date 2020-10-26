#pragma once
#include "types/instruction.h"
#include "utils/insts.h"

static const char *_test_inst_cset[] = {

};
static const char *_test_result_cset[] = {

};

static inline int _translate_cset(const struct instruction *inst, char *str, int *str_cnt, int len)
{
	char rd[8], op[20];
	int ret = sscanf(inst->string, "%*s\t" "%[^,], %s", rd, op);
	if (ret != 2) {
		return -1;
	}

	const char *cond[] = {"eq", "ne", "cs", "cc", "mi", "pl", "vs", "vc", "hi", "ls", "ge", "lt", "gt", "le"};
	const char *mean[] = {"==", "!=", "u>=", "u<", "<0", ">=0",
		"overflow", "not overflow", "u>", "u<=", "s>=", "s<", "s>", "s<=", "unknown"};

	int i = 0;
	for (i = 0; i < array_size(cond); i++) {
		if (strcmp(op, cond[i]) == 0) {
			break;
		}
	}

	char r1[20], r2[20];
	uint32_t address;
	if (get_cmp(&address, r1, r2)) {
		addr_printf("%s = (%s %s %s);", rd, r1, mean[i], r2);
	} else {
		addr_printf(_red("%s"), inst->string);
	}
	return 0;
}
