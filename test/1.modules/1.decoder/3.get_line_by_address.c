#include <stdio.h>
#include <string.h>
#include "modules/decoder.h"

int main()
{
	int ret = decoder_load("hhee.s");
	if (ret) {
		perror("load file hhee.s");
		return ret;
	}

	if (get_line_by_address(0x3f77c) != 30959) {
		return 1;
	}

	if (get_line_by_address(0) != 0) {
		return 2;
	}

	if (get_line_by_address(-4) != -1) {
		return 3;
	}

	if (get_line_by_address(8) != 2) {
		return 4;
	}

	if (get_line_by_address(9) != -1) {
		return 5;
	}

	decoder_unload();
	return 0;
}
