#pragma once
#include "types/instruction.h"
#include "utils/insts.h"
#include "types/bool.h"

static const char *_test_inst_ldrb[] = {
	"ldrb\tx0, [x1], #1", 0,
	"ldrb\tx0, [x1, #1]!", 0,
	"ldrb\tx0, [x1, #1]", 0,
	"ldrb\tx0, [x1, x2, uxtw #1]", 0,
	"ldrb\tx0, [x1, x2, uxtw]", 0,
	"ldrb\tx0, [x1, x2, lsl #12]", 0,
	"ldrb\tx0, [x1, x2]", 0,
	"ldrb\tx0, [x1]", 0,
};
static const char *_test_result_ldrb[] = {
	"x0 = ((uint8_t *)x1)[0];",
	"x1 += 1;", 0,
	"x1 += 1;",
	"x0 = ((uint8_t *)x1)[0];", 0,
	"x0 = ((uint8_t *)x1)[1];", 0,
	"x0 = ((uint8_t *)x1)[(uint32_t)(x2 << 1)]", 0,
	"x0 = ((uint8_t *)x1)[(uint32_t)(x2)]", 0,
	"x0 = ((uint8_t *)x1)[x2 << 12]", 0,
	"x0 = ((uint8_t *)x1)[x2]", 0,
	"x0 = ((uint8_t *)x1)[0];", 0,
};

static inline int _translate_ldrb(const struct instruction *inst, char *str, int *str_cnt, int len)
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
	} else {
		// rt, [rn{, #imm}]
		ret = sscanf(inst->string, "%*s\t"
				"%[^,], [%[^],]%[^.]", rt, rn, other);
		if (ret != 3) {
			return 1;
		}
		if (other[0] == ',') {
			// rt, [rn, #imm]
			ret = sscanf(other, ", #%d", &imm);
			if (ret != 1) {
				char rm[8], extend[8], amount[8];
				if (strstr(inst->string, "uxtw") != NULL ||
					strstr(inst->string, "sxtw") != NULL ||
					strstr(inst->string, "sxtx") != NULL) {
					// rt, [rn, rm, extend{ #amount}]

					if (strstr(inst->string, "#") != NULL) {
						// rt, [rn, rm, extend #amount]

						ret = sscanf(other, ", %[^,], %s #%[^]]]", rm, extend, amount);
						if (ret != 3) return 1;
						int datasize = 32;
						if (extend[3] == 'x') {
							datasize = 64;
						}
						addr_printf("%s = ((uint8_t *)%s)[(%sint%d_t)(%s << %s)];",
								rt, rn, extend[0] == 'u' ? "u" : "", datasize, rm, amount);
					} else {
						// rt, [rn, rm, extend]

						ret = sscanf(other, ", %[^,], %[^]]]", rm, extend);
						if (ret != 2) return 1;
						int datasize = 32;
						if (extend[3] == 'x') {
							datasize = 64;
						}
						addr_printf("%s = ((uint8_t *)%s)[(%sint%d_t)(%s)];",
								rt, rn, extend[0] == 'u' ? "u" : "", datasize, rm);
					}
					return 0;
				} else if (strstr(inst->string, "lsl") != NULL) {
					// rt, [rn, rm, lsl #amount]

					ret = sscanf(other, ", %[^,], lsl #%[^]]]", rm, amount);
					if (ret != 2) return 1;
					addr_printf("%s = ((uint8_t *)%s)[%s << %s];", rt, rn, rm, amount);
					return 0;
				} else {
					// rt, [rn, rm]

					ret = sscanf(other, ", %[^]]]", rm);
					if (ret != 1) return 1;
					addr_printf("%s = ((uint8_t *)%s)[%s];", rt, rn, rm);
					return 0;
				}
			}
		} else if (other[0] == ']') {
			// rt, [rn]
			imm = 0;
		} else {
			return 1;
		}

		preindex = false;
		postindex = false;
	}


	if (preindex) {
		addr_printf("%s += %d;\n", rn, imm);
		imm = 0;
	}

	if (!postindex) {
		addr_printf("%s = ((uint8_t *)%s)[%d];", rt, rn, imm);
	}

	if (postindex) {
		addr_printf("%s = ((uint8_t *)%s)[0];\n", rt, rn);
		addr_printf("%s += %d;", rn, imm);
	}
	return 0;
}
