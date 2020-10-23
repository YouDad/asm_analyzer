#pragma once
#include "types/instruction.h"
#include "utils/insts.h"
#include "types/bool.h"

static const char *_test_inst_ldr[] = {
	"ldr\tx0, [sp], #8", 0,
	"ldr\tx1, [sp, #-16]!", 0,
	"ldr\tx2, [sp, #48]", 0,
	"ldr\tx4, [x1, w6, lsl #3]", 0,
	"ldr\tx4, [x1, w6, uxtw #3]", 0,
	"ldr\tx4, [x1, w6, sxtx #3]", 0,
	"ldr\tx4, [x1, w6, sxtw #3]", 0,
	"ldr\tx4, [x1, w6, sxtw]", 0,
	"ldr\tx4, [x1, w6]", 0,
};
static const char *_test_result_ldr[] = {
	"x0 = (uint64_t *)sp[0];",
	"sp += 8;", 0,
	"sp += -16;",
	"x1 = (uint64_t *)sp[0];", 0,
	"x2 = (uint64_t *)sp[6];", 0,
	"x4 = (uint64_t *)x1[(uint64_t)(w6 << 3)];", 0,
	"x4 = (uint64_t *)x1[(uint32_t)(w6 << 3)];", 0,
	"x4 = (uint64_t *)x1[(int64_t)(w6 << 3)];", 0,
	"x4 = (uint64_t *)x1[(int32_t)(w6 << 3)];", 0,
	"x4 = (uint64_t *)x1[(int32_t)(w6)];", 0,
	"x4 = (uint64_t *)x1[w6];", 0,
};

// rt, [rn], #imm
// rt, [rn, #imm]!
// rt, [rn{, #imm}]
// rt, [rn, rm{, extend {#amount}}]
// rt, label
static inline int _translate_ldr(const struct instruction *inst, char *str, int *str_cnt, int len)
{
	char rt[8], rn[8], other[20];
	int ret, imm;
	bool preindex, postindex;

	if (strstr(inst->string, "],") != NULL) {
		// rt, [rn], #imm
		ret = sscanf(inst->string, "%*s\t"
				"%[^,], [%[^]]], #%d", rt, rn, &imm);
		if (ret != 3) {
			return 1;
		}

		preindex = false;
		postindex = true;
	} else if (strstr(inst->string, "]!") != NULL) {
		// rt, [rn, #imm]!
		ret = sscanf(inst->string, "%*s\t"
				"%[^,], [%[^,], #%d]!", rt, rn, &imm);
		if (ret != 3) {
			return 1;
		}

		preindex = true;
		postindex = false;
	} else if (strstr(inst->string, "[") == NULL) {
		// rt, label
		char label[20];
		ret = sscanf(inst->string, "%*s\t" "%[^,], %s", rt, label);
		if (ret != 2) {
			return 1;
		}

		addr_printf("%s = %s;", rt, label);
		return 0;
	} else {
		// rt, [rn{, #imm}]
		// rt, [rn, rm{, extend {#amount}}]
		ret = sscanf(inst->string, "%*s\t"
				"%[^,], [%[^],]%[^.]", rt, rn, other);
		if (ret != 3) {
			return 1;
		}

		if (other[0] == ',') {
			ret = sscanf(other, ", #%d", &imm);
			if (ret != 1) {
				int datasize = 32;
				if (rt[0] == 'x') {
					datasize = 64;
				}

				// rt, [rn, rm{, extend {#amount}}]
				char rm[8], extend[8], amount[8];
				bool have_extend = false;
				have_extend |= strstr(other, "lsl") != NULL;
				have_extend |= strstr(other, "uxtw") != NULL;
				have_extend |= strstr(other, "sxtw") != NULL;
				have_extend |= strstr(other, "sxtx") != NULL;

				if (have_extend) {
					bool have_imm = strstr(other, "#") != NULL;
					if (have_imm) {
						ret = sscanf(other, ", %[^,], %s #%[^]]]", rm, extend, amount);
						if (ret != 3) {
							return 1;
						}

						addr_printf("%s = (uint%d_t *)%s[(%s)(%s << %s)];",
								rt, datasize, rn, ext(extend, true), rm, amount);
					} else {
						ret = sscanf(other, ", %[^,], %s]", rm, extend);
						if (ret != 2) {
							return 1;
						}

						addr_printf("%s = (uint%d_t *)%s[(%s)(%s)];",
								rt, datasize, rn, ext(extend, true), rm);
					}
				} else {
					ret = sscanf(other, ", %[^]]]", rm);
					if (ret != 1) {
						return 1;
					}

					addr_printf("%s = (uint%d_t *)%s[%s];", rt, datasize, rn, rm);
				}
				return 0;
			}
		} else {
			imm = 0;
		}

		preindex = false;
		postindex = false;
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
		addr_printf("%s = (uint%d_t *)%s[%d];", rt, datasize, rn, imm * 8 / datasize);
	}

	if (postindex) {
		addr_printf("%s = (uint%d_t *)%s[0];", rt, datasize, rn);
		addr_printf("%s += %d;", rn, imm);
	}
	return 0;
}

