#include "types/instruction.h"
#include <stdio.h>
#include <string.h>
#include <malloc.h>

void print_instruction(struct instruction *i, char *pre)
{
	printf("%sinstruction{ addr: %04x, code: %x, string: \"%s\" }\n",
			pre, i->addr, i->code, i->string);
}

void print_instruction_block(struct instruction_block *ib)
{
	int ib_size = 4;
	ib_size += ib->end_addr - ib->start_addr;
	ib_size /= 4;

	printf("instruction_block[%d] {\n  start_addr: %x, end_addr: %x\n",
			ib_size, ib->start_addr, ib->end_addr);

	for (int i = 0; i < ib_size; i++) {
		print_instruction(&ib->ip[i], "  ");
	}

	printf("}\n");
}

void release_instruction_block(struct instruction_block *ib)
{
	free(ib->ip);
	free(ib);
}

int can_merge_instruction_block(struct instruction_block *prev, struct instruction_block *next)
{
	return prev->end_addr + 4 == next->start_addr;
}

void merge_instruction_block(struct instruction_block *prev, struct instruction_block *next)
{
	int prev_size = (prev->end_addr - prev->start_addr + 4) / 4;
	int next_size = (next->end_addr - next->start_addr + 4) / 4;
	int size = prev_size + next_size;

	prev->ip = (struct instruction *)realloc(prev->ip, size * sizeof(struct instruction));
	memcpy(&prev->ip[prev_size], next->ip, next_size * sizeof(struct instruction));
	prev->end_addr = next->end_addr;
}
