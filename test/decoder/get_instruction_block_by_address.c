#include <stdio.h>
#include "types.h"
#include "uint32_list_item.h"
#include "decoder.h"

int test(int sa, int ea, int *ja, int jlen)
{
	LIST_HEAD(address_queue);
	struct instruction_block ib;

	int ret = get_instruction_block_by_address(sa, &address_queue, &ib);
	if (ret < 0) {
		printf("get_instruction_block_by_address return failed(%d)\n", ret);
		return 1;
	}

	if (ib.start_address != sa) {
		printf("start_address assert failed(%x)\n", sa);
		return 2;
	}
	if (ib.end_address != ea) {
		printf("end_address assert failed(%x)\n", ea);
		return 3;
	}


	struct uint32_list_item *item, *tmp;
	int i = 0;
	list_for_each_entry_safe(item, tmp, &address_queue, list) {
		if (i >= jlen) {
			printf("jump_address length assert failed(%d)\n", jlen);
			return 4;
		}
		if (item->item != ja[i]) {
			printf("jump_address assert failed(%x, %x)\n", ja[i], item->item);
			return 5;
		}
		uint32_list_pop_head(&address_queue);
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

	analyzer_clean();
	return 0;
}
