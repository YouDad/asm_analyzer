#include "modules/analyzer.h"
#include "modules/translater.h"

int test(const char **insts, int insts_len, const char **strs, int strs_len);
int main()
{
#define inst_define(id) const char *is##id[] =
#define tstr_define(id) const char *s##id[] =
#define test_______(id) do {\
	int ret = test(is##id, array_size(is##id), s##id, array_size(s##id));\
	if (ret) {       \
		return ret;  \
	}                \
} while (0)

	inst_define(1) { "mov\tw0, #0x1", };
	tstr_define(1) { "w0 = 0x1;", };
	test_______(1);

	inst_define(2) { "stp\tx29, x30, [sp, #-32]!", };
	tstr_define(2) {
		"sp += -32;",
		"(uint64_t *)sp[0] = x29;",
		"(uint64_t *)sp[1] = x30;",
	};
	test_______(2);

	inst_define(3) { "str\tx0, [sp, #16]", };
	tstr_define(3) { "(uint64_t *)sp[2] = x0;", };
	test_______(3);

	inst_define(4) { "adrp\tx0, c000", };
	tstr_define(4) { "x0 = c000;", };
	test_______(4);

	inst_define(5) { "add\tx0, x0, #1", };
	tstr_define(5) { "x0 = x0 + 1;", };
	test_______(5);

	inst_define(6) { "bl\tde0", };
	tstr_define(6) { "text_0xde0();", };
	test_______(6);

	inst_define(7) { "b.eq\td9c", };
	tstr_define(7) { "if == then goto", };
	test_______(7);

	inst_define(8) { "ldrb\tw0, [x0]", };
	tstr_define(8) { "w0 = (uint8_t *)x0[0];", };
	test_______(8);

	inst_define(9) { "cbnz\tw0, 123", };
	tstr_define(9) { "if w0 != 0 then goto", };
	test_______(9);

	inst_define(10) { "cbz\tw0, 123", };
	tstr_define(10) { "if w0 == 0 then goto", };
	test_______(10);

	inst_define(11) { "b\t123", };
	tstr_define(11) { "goto ", };
	test_______(11);

	inst_define(12) { "b.ne\td9c", };
	tstr_define(12) { "if != then goto", };
	test_______(12);

	inst_define(13) { "b.cs\td9c", };
	tstr_define(13) { "if u>= then goto", };
	test_______(13);

	inst_define(14) { "b.cc\td9c", };
	tstr_define(14) { "if u< then goto", };
	test_______(14);

	inst_define(15) { "b.mi\td9c", };
	tstr_define(15) { "if <0 then goto", };
	test_______(15);

	inst_define(16) { "b.pl\td9c", };
	tstr_define(16) { "if >=0 then goto", };
	test_______(16);

	inst_define(17) { "b.vs\td9c", };
	tstr_define(17) { "if overflow then goto", };
	test_______(17);

	inst_define(18) { "b.vc\td9c", };
	tstr_define(18) { "if not overflow then goto", };
	test_______(18);

	inst_define(19) { "b.hi\td9c", };
	tstr_define(19) { "if u> then goto", };
	test_______(19);

	inst_define(20) { "b.ls\td9c", };
	tstr_define(20) { "if u<= then goto", };
	test_______(20);

	inst_define(21) { "b.ge\td9c", };
	tstr_define(21) { "if s>= then goto", };
	test_______(21);

	inst_define(22) { "b.lt\td9c", };
	tstr_define(22) { "if s< then goto", };
	test_______(22);

	inst_define(23) { "b.gt\td9c", };
	tstr_define(23) { "if s> then goto", };
	test_______(23);

	inst_define(24) { "b.le\td9c", };
	tstr_define(24) { "if s<= then goto", };
	test_______(24);

	inst_define(25) { "b.aa\td9c", };
	tstr_define(25) { "if unknown then goto", };
	test_______(25);

	inst_define(26) { "sub\tx0, x1, x2", };
	tstr_define(26) { "x0 = x1 - x2;", };
	test_______(26);

	inst_define(27) { "sub\tx0, x1, x2, uxtw #1", };
	tstr_define(27) { "x0 = x1 - (uint32_t)(x2 << 1);", };
	test_______(27);

	inst_define(28) { "sub\tx0, x1, x2, sxtx", };
	tstr_define(28) { "x0 = x1 - (int64_t)x2;", };
	test_______(28);

	inst_define(29) { "sub\tx9, x30, sp, lsl #32", };
	tstr_define(29) { "x9 = x30 - (uint64_t)(sp << 32);", };
	test_______(29);

	inst_define(30) { "add\tx0, x1, x2", };
	tstr_define(30) { "x0 = x1 + x2;", };
	test_______(30);

	inst_define(31) { "add\tx0, x1, x2, uxtw #1", };
	tstr_define(31) { "x0 = x1 + (uint32_t)(x2 << 1);", };
	test_______(31);

	inst_define(32) { "add\tx0, x1, x2, sxtx", };
	tstr_define(32) { "x0 = x1 + (int64_t)x2;", };
	test_______(32);

	inst_define(33) { "add\tx9, x30, sp, lsl #32", };
	tstr_define(33) { "x9 = x30 + (uint64_t)(sp << 32);", };
	test_______(33);

	return 0;
}

char str[1 << 16];

int test(const char **insts, int insts_len, const char **strs, int strs_len)
{
	LIST_HEAD(iblist);
	struct instruction_block *ib = MALLOC(struct instruction_block, 1);

	ib->start_addr = 0;
	ib->end_addr = insts_len * 4 - 4;
	ib->ip = MALLOC(struct instruction, insts_len);
	for (int i = 0; i < insts_len; i++) {
		ib->ip[i].addr = i * 4;
		ib->ip[i].code = 0;
		strcpy(ib->ip[i].string, insts[i]);
	}
	list_insert_tail(&ib->list, &iblist);
	int ret = translate(&iblist, str, sizeof(str));
	if (ret) {
		printf("translate failed(%d)\n", ret);
		return ret;
	}

	for (int i = 0; i < strs_len; i++) {
		if (strstr(str, strs[i]) == NULL) {
			printf("\"%s\"\n", str);
			printf("\"%s\"\nassert strstr failed\n", strs[i]);
			return 2;
		}
	}

	return 0;
}
