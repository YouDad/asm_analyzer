#include "modules/analyzer.h"

int test(int addr, int *a, int alen);
int main()
{
	int ret = decoder_load("hhee.s");
	if (ret) {
		perror("load file hhee.s");
		return ret;
	}

	int a1[] = {0x90, 0xa608, 0x2f78};
	ret = test(0, a1, sizeof(a1));
	if (ret) {
		printf("test a1 failed(%d)\n", ret);
		return 1;
	}

	int a2[] = {0xa578, 0x90};
	ret = test(0x110, a2, sizeof(a2));
	if (ret) {
		printf("test a2 failed(%d)\n", ret);
		return 2;
	}

	int a3[] = {0x872c, 0xa470, 0xde0, 0x872c, 0xa470, 0xde0, 0x8880,
		0xa4e0, 0x8880, 0xa4e0, 0x47c, 0xde0, 0xde0, 0xbaf8, 0x47c,
		0x113c, 0x25f4, 0xda8, 0xf24, 0xdb4, 0x872c, 0xa470, 0xa470,
		0x7cb4, 0xccc, 0x10a0, 0xccc, 0xde0, 0xd54, 0xd54, 0x7cb4,
		0x79d8, 0xd54, 0xd54, 0xd54, 0xd54, 0xd54, 0xd54, 0xd54, 0xd54,
		0xd54, 0xd54, 0xd54, 0xd54, 0x7fcc, 0xb74, 0x7c78, 0xccc, 0x10a0,
		0xde0, 0xb74, 0xb74, 0xd54, 0x7c78, 0xd54, 0xd54, 0xd54};
	ret = test(0x2e0, a3, sizeof(a3));
	if (ret) {
		printf("test a3 failed(%d)\n", ret);
		return 3;
	}

	decoder_unload();
	return 0;
}

int test(int addr, int *a, int alen)
{
	uint32_list_define(callee_addrs);

	int ret = get_callee_by_addr(addr, &callee_addrs);
	if (ret) {
		printf("get_callee_by_addr failed(%d)\n", ret);
		return 1;
	}

	int i = 0;
	while (!uint32_list_empty(&callee_addrs)) {
		if (i >= alen) {
			printf("alen assert failed(%d)\n", alen);
			return 2;
		}

		uint32_t addr = uint32_list_pop(&callee_addrs);
		if (a[i] != addr) {
			printf("addr assert failed(%x, %x)\n", a[i], addr);
			return 3;
		}

		i++;
	}

	return 0;
}
