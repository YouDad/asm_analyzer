#pragma once

#include "list.h"
#include "vector.h"
#include "uint32_list_item.h"
#include "bitmap.h"
#include <stdio.h>

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
