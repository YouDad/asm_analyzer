#include "modules/decoder.h"

int get_callee_by_address(uint32_t address,
		struct uint32_list *callee_addr_list);
int init_call_graph(struct uint32_list *addr_queue);
void fini_call_graph();
void get_dest_funcs(struct uint32_list *addr_list);
