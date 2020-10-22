#include "modules/analyzer.h"
#include "modules/translater.h"
#include "insts/armv8.h"

int test(const char **insts, int insts_len, const char **strs, int strs_len);
int main()
{
	for (int i = 0; i < array_size(armv8_insts); i++) {
		struct asm_inst *in = &armv8_insts[i];
		int is = 0, ie, rs = 0, re;
		while (is < in->test_inst_len) {
			for (ie = is; ie < in->test_inst_len; ie++) {
				if (in->test_inst[ie] == 0) {
					break;
				}
			}

			for (re = rs; re < in->test_result_len; re++) {
				if (in->test_result[re] == 0) {
					break;
				}
			}

			int ret = test(in->test_inst + is, ie - is,
					in->test_result + rs, re - rs);
			if (ret) {
				return ret;
			}

			is = ie + 1;
			rs = re + 1;
		}
	}

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
			printf("\"%s\"\n", strs[i]);
			printf("assert strstr(%s) failed\n", insts[0]);
			return 2;
		}
	}

	return 0;
}
