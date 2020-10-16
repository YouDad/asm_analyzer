#include <stdio.h>
#include <string.h>
#include "decoder.h"

int main()
{
	int ret = decoder_load("hhee.s");
	if (ret) {
		perror("load file hhee.s");
		return ret;
	}

	struct instruction instr;
	instr = get_instruction_by_line(7);
	if (instr.address != 0x0) return 1;
	if (instr.code != 0xd280001d) return 2;
	if (strcmp("mov\tx29, #0x0", instr.string) != 0) return 3;
	instr = get_instruction_by_line(9);
	if (instr.address != 0x8) return 4;
	if (instr.code != 0x72a618a8) return 5;
	if (strcmp("movk\tw8, #0x30c5, lsl #16", instr.string) != 0) return 6;
	instr = get_instruction_by_line(24);
	if (instr.address != 0x44) return 7;
	if (instr.code != 0xf0002e62) return 8;
	if (strcmp("adrp\tx2, 5cf000 <.text+0x5cf000>", instr.string) != 0) return 9;
	instr = get_instruction_by_line(30000);
	if (instr.address != 0x3e774) return 10;
	if (instr.code != 0x00000000) return 11;
	if (strcmp(".inst\t0x00000000 ; undefined", instr.string) != 0) return 12;
	instr = get_instruction_by_line(30001);
	if (instr.address != 0x3e778) return 13;
	if (instr.code != 0x00def002) return 14;
	if (strcmp(".inst\t0x00def002 ; undefined", instr.string) != 0) return 15;

	analyzer_clean();
	return 0;
}
