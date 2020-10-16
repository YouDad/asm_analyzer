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

struct instructions {
	struct list_head list;
	int start_address;
	int end_address;
	struct instruction *ip;
};
static inline void print_instructions(struct instructions *i10s)
{
	int i10s_size = 4;
	i10s_size += i10s->end_address - i10s->start_address;
	i10s_size /= 4;

	printf("instructions[%d] {\n  start_address: %x, end_address: %x\n",
			i10s_size, i10s->start_address, i10s->end_address);

	for (int i = 0; i < i10s_size; i++) {
		print_instruction(&i10s->ip[i], "  ");
	}

	printf("}\n");
}
static inline void release_instructions(struct instructions *i10s)
{
	free(i10s->ip);
	free(i10s);
}
static inline int can_merge_instructions(struct instructions *prev, struct instructions *next)
{
	return prev->end_address + 4 == next->start_address;
}
static inline void merge_instructions(struct instructions *prev, struct instructions *next)
{
	int prev_size = (prev->end_address - prev->start_address + 4) / 4;
	int next_size = (next->end_address - next->start_address + 4) / 4;
	int size = prev_size + next_size;

	prev->ip = (struct instruction *)realloc(prev->ip, size * sizeof(struct instruction));
	memcpy(&prev->ip[prev_size], next->ip, next_size * sizeof(struct instruction));
	prev->end_address = next->end_address;
}
