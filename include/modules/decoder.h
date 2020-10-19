#include "common.h"
#include "types.h"

vector_define_extern(char *, lines);

int decoder_load(char *filename);
void decoder_unload();
int get_address_by_line(uint32_t line);
int get_line_by_address(uint32_t address);
struct instruction get_instruction_by_line(uint32_t line);
int get_instruction_block_by_address(uint32_t start_address,
		struct uint32_list *address_queue,
		struct instruction_block *retval);
int get_function_by_address(uint32_t start_address,
		struct list_head *instruction_block_list);
