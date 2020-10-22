#pragma once
#include "types/instruction.h"
#include "utils/insts.h"

static const char *_test_inst_b_cond[] = {
	"b.eq\td9c", 0,
	"b.ne\td9c", 0,
	"b.cs\td9c", 0,
	"b.cc\td9c", 0,
	"b.mi\td9c", 0,
	"b.pl\td9c", 0,
	"b.vs\td9c", 0,
	"b.vc\td9c", 0,
	"b.hi\td9c", 0,
	"b.ls\td9c", 0,
	"b.ge\td9c", 0,
	"b.lt\td9c", 0,
	"b.gt\td9c", 0,
	"b.le\td9c", 0,
	"b.aa\td9c", 0,
};
static const char *_test_result_b_cond[] = {
	"if == then goto", 0,
	"if != then goto", 0,
	"if u>= then goto", 0,
	"if u< then goto", 0,
	"if <0 then goto", 0,
	"if >=0 then goto", 0,
	"if overflow then goto", 0,
	"if not overflow then goto", 0,
	"if u> then goto", 0,
	"if u<= then goto", 0,
	"if s>= then goto", 0,
	"if s< then goto", 0,
	"if s> then goto", 0,
	"if s<= then goto", 0,
	"if unknown then goto", 0,
};

static inline int _translate_b_cond(const struct instruction *inst, char *str, int *str_cnt, int len)
{
	char op[8], addr[20];
	int ret = sscanf(inst->string, "b.%s\t" "%s", op, addr);
	if (ret != 2) {
		return 1;
	}

	const char *cond[] = {"eq", "ne", "cs", "cc", "mi",
		"pl", "vs", "vc", "hi", "ls", "ge", "lt", "gt", "le"};
	const char *mean[] = {"==", "!=", "u>=", "u<", "<0", ">=0",
		"overflow", "not overflow", "u>", "u<=", "s>=", "s<", "s>", "s<=", "unknown"};

	int i = 0;
	for (i = 0; i < array_size(cond); i++) {
		if (strcmp(op, cond[i]) == 0) {
			break;
		}
	}

	addr_printf("if %s then goto ", mean[i]);
	return 0;
}
