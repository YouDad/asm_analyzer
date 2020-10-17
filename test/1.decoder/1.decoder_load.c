#include <stdio.h>
#include <string.h>
#include "libs/decoder.h"

int main()
{
	int ret = decoder_load("hhee.s");
	if (ret) {
		perror("load file hhee.s");
		return ret;
	}

	if (vector_size(lines) != 31028) {
		return 1;
	}

	if (strstr(lines[31025], "00fef002") == 0) {
		return 2;
	}

	analyzer_clean();
	return 0;
}
