#include "list.h"
#include <stdio.h>
#include <string.h>
#include <malloc.h>

struct instruction {
	int address;
	unsigned int code;
	char string[40];
};
static inline void print_instruction(struct instruction *i, char *pre)
{
	printf("%sinstruction{ address: %04x, code: %x, string: \"%s\" }\n",
			pre, i->address, i->code, i->string);
}

struct instruction_block {
	struct list_head list;
	int start_address;
	int end_address;
	struct instruction *ip;
};
static inline void print_instruction_block(struct instruction_block *ib)
{
	int ib_size = 4;
	ib_size += ib->end_address - ib->start_address;
	ib_size /= 4;

	printf("instruction_block[%d] {\n  start_address: %x, end_address: %x\n",
			ib_size, ib->start_address, ib->end_address);

	for (int i = 0; i < ib_size; i++) {
		print_instruction(&ib->ip[i], "  ");
	}

	printf("}\n");
}
static inline void release_instruction_block(struct instruction_block *ib)
{
	free(ib->ip);
	free(ib);
}
static inline int can_merge_instruction_block(struct instruction_block *prev, struct instruction_block *next)
{
	return prev->end_address + 4 == next->start_address;
}
static inline void merge_instruction_block(struct instruction_block *prev, struct instruction_block *next)
{
	int prev_size = (prev->end_address - prev->start_address + 4) / 4;
	int next_size = (next->end_address - next->start_address + 4) / 4;
	int size = prev_size + next_size;

	prev->ip = (struct instruction *)realloc(prev->ip, size * sizeof(struct instruction));
	memcpy(&prev->ip[prev_size], next->ip, next_size * sizeof(struct instruction));
	prev->end_address = next->end_address;
}
