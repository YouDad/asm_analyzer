#include <stdio.h>
#include "modules/decoder.h"

int test(int addr, int *sa, int *ea, int len)
{
	LIST_HEAD(i10s_list);
	struct instruction_block *item, *tmp;

	int ret = get_function_by_address(addr, &i10s_list);
	if (ret) {
		printf("get_function_by_address failed(%d)\n", ret);
		return 1;
	}

	int i = 0;
	list_for_each_entry_safe(item, tmp, &i10s_list, list) {
		if (i >= len) {
			printf("address length assert failed(%d)\n", len);
			print_instruction_block(item);
			return 2;
		}
		if (item->start_address != sa[i]) {
			printf("start_address assert failed(%x, %x)\n", sa[i], item->start_address);
			print_instruction_block(item);
			return 3;
		}
		if (item->end_address != ea[i]) {
			printf("end_address assert failed(%x, %x)\n", ea[i], item->end_address);
			print_instruction_block(item);
			return 4;
		}
		list_del(&item->list);
		release_instruction_block(item);
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

	LIST_HEAD(i10s_list);
	struct instruction_block *item, *tmp;

	int sa1[] = {0x0};
	int ea1[] = {0x70};
	ret = test(0, sa1, ea1, sizeof(sa1));
	if (ret) return ret;

	int sa2[] = {0xb808};
	int ea2[] = {0xb850};
	ret = test(0xb808, sa2, ea2, sizeof(sa2));
	if (ret) return ret;

	int sa3[] = {0xb8cc};
	int ea3[] = {0xba64};
	ret = test(0xb8cc, sa3, ea3, sizeof(sa3));
	if (ret) return ret;

	int sa4[] = {0xa8, 0xbb64};
	int ea4[] = {0x10c, 0xbb68};
	ret = test(0xbb64, sa4, ea4, sizeof(sa4));
	if (ret) return ret;

	decoder_unload();
	return 0;
}
