#include "modules/decoder.h"

int get_called_func_by_address(uint32_t address,
		struct list_head *called_address_list);
int init_call_graph(struct list_head *addr_queue);
void fini_call_graph();
