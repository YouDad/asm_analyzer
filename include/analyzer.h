#include "types.h"
vector_define_extern(char *, lines);

int analyzer_load(char *filename);
void analyzer_clean();
int get_address_by_line(uint32_t line);
int get_line_by_address(uint32_t address);
struct instruction get_instruction_by_line(uint32_t line);
int get_instructions_by_address(uint32_t start_address,
		struct list_head *address_queue,
		struct instructions *retval);
int get_function_by_address(uint32_t start_address,
		struct list_head *instructions_list);
