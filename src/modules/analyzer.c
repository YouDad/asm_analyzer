#include "modules/analyzer.h"
#include "modules/addr_graph.h"

int get_callee_by_address(uint32_t address,
		struct uint32_list *callee_addr_list)
{
	LIST_HEAD(iblist);

	int ret = get_function_by_address(address, &iblist);
	if (ret) {
		return ret;
	}

	struct instruction_block *item, *tmp;
	list_for_each_entry_safe(item, tmp, &iblist, list) {
		struct instruction *i = item->ip;

		while (1) {
			if (strstr(i->string, "bl") == i->string) {
				// bl <addr>
				uint32_t new_address;
				int ret = sscanf(i->string, "%*s%x", &new_address);
				if (ret != 1) {
					return -EINTERNAL;
				}
				uint32_list_push(callee_addr_list, new_address);
			}
			if (i->address < item->end_address) {
				i++;
			} else {
				break;
			}
		}
	}

	return 0;
}

int init_call_graph(struct uint32_list *addr_queue)
{
	addr_graph_init();

	while (!uint32_list_empty(addr_queue)) {
		uint32_t caller_addr = uint32_list_pop(addr_queue);
		uint32_list_define(aq);

		int ret = get_callee_by_address(caller_addr, &aq);
		if (ret) {
			return ret;
		}

		while (!uint32_list_empty(&aq)) {
			uint32_t callee_addr = uint32_list_pop(&aq);
			addr_graph_add_call(caller_addr, callee_addr);
			uint32_list_push(addr_queue, callee_addr);
		}
	}

	addr_graph_fixup();
	return 0;
}

void fini_call_graph()
{
	addr_graph_fini();
}
