#pragma once
#include "types/list.h"
#include "types/uint32_list.h"

struct instruction {
	int addr;
	unsigned int code;
	char string[40];
};

void print_instruction(struct instruction *i, char *pre);

struct instruction_block {
	struct list_head list;
	int start_addr;
	int end_addr;
	struct instruction *ip;
};

void print_instruction_block(struct instruction_block *ib);
void release_instruction_block(struct instruction_block *ib);
int can_merge_instruction_block(struct instruction_block *prev, struct instruction_block *next);
void merge_instruction_block(struct instruction_block *prev, struct instruction_block *next);

int regs_affected_by_inst(struct instruction *i, struct uint32_list *r, struct uint32_list *w);
