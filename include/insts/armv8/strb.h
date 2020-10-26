#pragma once
#include "types/instruction.h"
#include "utils/insts.h"
#include "types/bool.h"

static const char *_test_inst_strb[] = {
	"strb\tx0, [sp], #8", 0,
	"strb\tx1, [sp, #-16]!", 0,
	"strb\tx2, [sp, #48]", 0,
	"strb\tx4, [x1, w6, lsl #3]", 0,
	"strb\tx4, [x1, w6, uxtw #3]", 0,
	"strb\tx4, [x1, w6, sxtx #3]", 0,
	"strb\tx4, [x1, w6, sxtw #3]", 0,
	"strb\tx4, [x1, w6, sxtw]", 0,
	"strb\tx4, [x1, w6]", 0,
};
static const char *_test_result_strb[] = {
	"((uint8_t *)sp)[0] = x0;",
	"sp += 8;", 0,
	"sp += -16;",
	"((uint8_t *)sp)[0] = x1;", 0,
	"((uint8_t *)sp)[48] = x2;", 0,
	"((uint8_t *)x1)[(uint64_t)(w6 << 3)] = x4;", 0,
	"((uint8_t *)x1)[(uint32_t)(w6 << 3)] = x4;", 0,
	"((uint8_t *)x1)[(int64_t)(w6 << 3)] = x4;", 0,
	"((uint8_t *)x1)[(int32_t)(w6 << 3)] = x4;", 0,
	"((uint8_t *)x1)[(int32_t)(w6)] = x4;", 0,
	"((uint8_t *)x1)[w6] = x4;", 0,
};

// rt, [rn], #imm
// rt, [rn, #imm]!
// rt, [rn{, #imm}]
// rt, [rn, rm{, extend {#amount}}]
static inline int _translate_strb(const struct instruction *inst, char *str, int *str_cnt, int len)
{
	char rt[8], rn[8], other[20];
	int ret, imm;
	bool preindex, postindex;

	if (strstr(inst->string, "],") != NULL) {
		// rt, [rn], #imm
		ret = sscanf(inst->string, "%*s\t"
				"%[^,], [%[^]]], #%d", rt, rn, &imm);
		if (ret != 3) {
			return -1;
		}

		preindex = false;
		postindex = true;
	} else if (strstr(inst->string, "]!") != NULL) {
		// rt, [rn, #imm]!
		ret = sscanf(inst->string, "%*s\t"
				"%[^,], [%[^,], #%d]!", rt, rn, &imm);
		if (ret != 3) {
			return -1;
		}

		preindex = true;
		postindex = false;
	} else {
		// rt, [rn{, #imm}]
		// rt, [rn, rm{, extend {#amount}}]
		ret = sscanf(inst->string, "%*s\t"
				"%[^,], [%[^],]%[^.]", rt, rn, other);
		if (ret != 3) {
			return -1;
		}

		if (other[0] == ',') {
			ret = sscanf(other, ", #%d", &imm);
			if (ret != 1) {
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
							return -1;
						}

						addr_printf("((uint8_t *)%s)[(%s)(%s << %s)] = %s;",
								rn, ext(extend, true), rm, amount, rt);
					} else {
						ret = sscanf(other, ", %[^,], %s]", rm, extend);
						if (ret != 2) {
							return -1;
						}

						addr_printf("((uint8_t *)%s)[(%s)(%s)] = %s;",
								rn, ext(extend, true), rm, rt);
					}
				} else {
					ret = sscanf(other, ", %[^]]]", rm);
					if (ret != 1) {
						return -1;
					}

					addr_printf("((uint8_t *)%s)[%s] = %s;", rn, rm, rt);
				}
				return 0;
			}
		} else {
			imm = 0;
		}

		preindex = false;
		postindex = false;
	}

	if (preindex) {
		addr_printf("%s += %d;\n", rn, imm);
		imm = 0;
	}

	if (!postindex) {
		addr_printf("((uint8_t *)%s)[%d] = %s;", rn, imm, rt);
	}

	if (postindex) {
		addr_printf("((uint8_t *)%s)[0] = %s;", rn, rt);
		addr_printf("%s += %d;", rn, imm);
	}
	return 0;
}
