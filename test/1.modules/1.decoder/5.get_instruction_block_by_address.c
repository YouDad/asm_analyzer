#include <stdio.h>
#include "modules/decoder.h"

int test(int sa, int ea, int *ja, int jlen)
{
	uint32_list_define(addr_queue);
	struct instruction_block ib;

	int ret = get_instruction_block_by_addr(sa, &addr_queue, &ib);
	if (ret < 0) {
		printf("get_instruction_block_by_addr return failed(%d)\n", ret);
		return 1;
	}

	if (ib.start_addr != sa) {
		printf("start_addr assert failed(%x)\n", sa);
		return 2;
	}
	if (ib.end_addr != ea) {
		printf("end_addr assert failed(%x)\n", ea);
		return 3;
	}

	int i = 0;
	while (!uint32_list_empty(&addr_queue)) {
		if (i >= jlen) {
			printf("jump_addr length assert failed(%d)\n", jlen);
			return 4;
		}
		int addr = uint32_list_pop(&addr_queue);
		if (addr != ja[i]) {
			printf("jump_addr assert failed(%x, %x)\n", ja[i], addr);
			return 5;
		}
		i++;
	}

	return 0;
}

int main()
{
	int ret = decoder_load("hhee.s");
	if (ret) {
		perror("load file hhee.s");
		return ret;
	}

	int ja1[0] = {};
	ret = test(0x0, 0x70, ja1, 0);
	if (ret) {
		return ret;
	}

	int ja2[1] = {0xde0};
	ret = test(0x190, 0x1a8, ja2, 1);
	if (ret) {
		return ret;
	}

	int ja3[2] = {0x240, 0x260};
	ret = test(0x1ac, 0x23c, ja3, 2);
	if (ret) {
		return ret;
	}

	decoder_unload();
	return 0;
}
