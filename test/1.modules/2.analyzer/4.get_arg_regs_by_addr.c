#include "modules/analyzer.h"

int test(uint32_t addr, uint32_t arg_regs);
int main()
{
	int ret = decoder_load("hhee.s");
	if (ret) {
		perror("load file hhee.s");
		return ret;
	}

	uint32_list_define(aq);
	uint32_list_push(&aq, 0);

	ret = init_call_graph(&aq);
	if (ret) {
		printf("init_call_graph failed(%d)\n", ret);
		return 1;
	}

	test(0xbac4, 0x3);
	test(0xbb2c, 0x1);

	fini_call_graph();
	decoder_unload();
	return 0;
}

int test(uint32_t addr, uint32_t arg_regs)
{
	uint32_t ar;
	int ret = get_arg_regs_by_addr(addr, &ar);
	if (ret) {
		printf("get_arg_regs_by_addr failed(%d)\n", ret);
		return 2;
	}

	if (arg_regs != ar) {
		printf("arl assert failed(%x != target: %x)\n", ar, arg_regs);
		return 3;
	}

	return 0;
}
