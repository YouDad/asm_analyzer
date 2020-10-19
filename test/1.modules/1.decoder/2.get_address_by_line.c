#include <stdio.h>
#include "modules/decoder.h"

int main()
{
	int ret = decoder_load("hhee.s");
	if (ret) {
		perror("load file hhee.s");
		return ret;
	}

	if (get_addr_by_line(0) != 0) {
		return 1;
	}

	if (get_addr_by_line(1) != 4) {
		return 2;
	}

	if (get_addr_by_line(2) != 8) {
		return 3;
	}

	if (get_addr_by_line(3) != 12) {
		return 4;
	}

	if (get_addr_by_line(4) != 16) {
		return 5;
	}

	if (get_addr_by_line(5) != 20) {
		return 6;
	}

	if (get_addr_by_line(6) != 24) {
		return 7;
	}

	if (get_addr_by_line(7) != 28) {
		return 8;
	}

	if (get_addr_by_line(8) != 32) {
		return 9;
	}

	if (get_addr_by_line(9) != 36) {
		return 10;
	}

	if (get_addr_by_line(30959) != 0x3f77c) {
		return 11;
	}

	decoder_unload();
	return 0;
}
