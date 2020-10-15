#include <stdio.h>
#include <string.h>
#include "analyzer.h"

int main()
{
	int ret = analyzer_load("hhee.s");
	if (ret) {
		perror("load file hhee.s");
		return ret;
	}

	if (get_line_by_address(0x3f77c) != 31026) {
		return 1;
	}

	if (get_line_by_address(0) != 7) {
		return 2;
	}

	if (get_line_by_address(-4) != -1) {
		return 3;
	}

	if (get_line_by_address(8) != 9) {
		return 4;
	}

	if (get_line_by_address(9) != -1) {
		return 5;
	}

	analyzer_clean();
	return 0;
}
