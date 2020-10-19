#include "modules/analyzer.h"

int main()
{
	int ret = decoder_load("hhee.s");
	if (ret) {
		perror("load file hhee.s");
		return ret;
	}

	uint32_list_define(al);
	uint32_list_define(aq);
	uint32_list_push(&aq, 0);

	ret = init_call_graph(&aq);
	if (ret) {
		printf("init_call_graph failed(%d)\n", ret);
		return 1;
	}

	get_caller_by_addr(0xbb2c, &al);

	int sum = 0xa548 + 0xa530;
	while (!uint32_list_empty(&al)) {
		sum -= uint32_list_pop(&al);
	}

	if (sum) {
		printf("sum assert failed(%d)\n", sum);
		return 2;
	}

	fini_call_graph();
	decoder_unload();
	return 0;
}
