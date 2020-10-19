#include "modules/decoder.h"

int get_called_func_by_address(uint32_t address,
		struct uint32_list *called_address_list);
int init_call_graph(struct uint32_list *addr_queue);
void fini_call_graph();
