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
static inline void print_instruction(struct instruction *i)
{
	printf("instruction{ address: %x, code: %x, string: \"%s\" }\n",
			i->address, i->code, i->string);
}

struct instructions {
	struct list_head list;
	int start_address;
	int end_address;
	struct instruction *ip;
};
static inline void print_instructions(struct instructions *instrs)
{
	int instrs_size = 4;
	instrs_size += instrs->end_address - instrs->start_address;
	instrs_size /= 4;

	printf("instructions[%d] {\nstart_address: %x, end_address: %x\n",
			instrs_size, instrs->start_address, instrs->end_address);

	for (int i = 0; i < instrs_size; i++) {
		print_instruction(&instrs->ip[i]);
	}

	printf("}\n");
}
