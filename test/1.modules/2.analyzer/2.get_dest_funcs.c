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

	int sum = 0x0090 + 0x0190;
	sum += 0x0280 + 0x047c + 0x0da8 + 0x0db4 + 0x0f48;
	sum += 0x1194 + 0x2dd4 + 0x2ebc + 0x2f38 + 0x7c78;
	sum += 0x7cb4 + 0x7fcc + 0x8680 + 0x872c + 0x8880;
	sum += 0x88cc + 0xa608 + 0xb800 + 0xb854 + 0xba68;
	sum += 0xba88 + 0xba90 + 0xbaa4 + 0xbaf8 + 0xbb10;

	get_dest_funcs(&al);
	while (!uint32_list_empty(&al)) {
		uint32_t addr = uint32_list_pop(&al);
		sum -= addr;
	}

	if (sum) {
		printf("sum assert failed(%d)\n", sum);
		return 2;
	}

	fini_call_graph();
	decoder_unload();
	return 0;
}
