#pragma once
#include "common.h"
#include "types.h"

vector_define_extern(char *, lines);

int decoder_load(char *filename);
void decoder_unload();
int get_addr_by_line(uint32_t line);
int get_line_by_addr(uint32_t addr);
struct instruction get_instruction_by_line(uint32_t line);
int get_instruction_block_by_addr(uint32_t start_addr,
		struct uint32_list *addr_queue,
		struct instruction_block *retval);
int get_func_by_addr(uint32_t start_addr, int clear,
		struct list_head *instruction_block_list);
