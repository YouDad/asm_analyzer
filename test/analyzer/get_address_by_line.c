#include <stdio.h>
#include "analyzer.h"

int main()
{
	int ret = analyzer_load("hhee.s");
	if (ret) {
		perror("load file hhee.s");
		return ret;
	}

	if (get_address_by_line(0) != -1) {
		return 1;
	}

	if (get_address_by_line(1) != -1) {
		return 2;
	}

	if (get_address_by_line(2) != -1) {
		return 3;
	}

	if (get_address_by_line(3) != -1) {
		return 4;
	}

	if (get_address_by_line(4) != -1) {
		return 5;
	}

	if (get_address_by_line(5) != -1) {
		return 6;
	}

	if (get_address_by_line(6) != -1) {
		return 7;
	}

	if (get_address_by_line(7) != 0) {
		return 8;
	}

	if (get_address_by_line(8) != 4) {
		return 9;
	}

	if (get_address_by_line(9) != 8) {
		return 10;
	}

	if (get_address_by_line(31026) != 0x3f77c) {
		return 11;
	}

	analyzer_clean();
	return 0;
}
