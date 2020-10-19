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

	if (vector_size(lines) != 30960) {
		return 1;
	}

	if (strstr(lines[30958], "00fef002") == 0) {
		return 2;
	}

	decoder_unload();
	return 0;
}
