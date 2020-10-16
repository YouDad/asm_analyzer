#include "types.h"
vector_define_extern(char *, lines);

int analyzer_load(char *filename);
int get_address_by_line(int i);
int get_line_by_address(int address);
int get_instructions_by_address(int start_address,
		struct list_head *address_queue,
		struct instructions *retval);
void analyzer_clean();
struct instruction get_instruction_by_line(int line);
