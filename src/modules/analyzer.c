#include "modules/analyzer.h"
#include "modules/addr_graph.h"

int get_called_func_by_address(uint32_t address,
		struct list_head *called_address_list)
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
				uint32_list_insert_tail(called_address_list, new_address);
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

int init_call_graph(struct list_head *addr_queue)
{
	addr_graph_init();

	while (!list_empty(addr_queue)) {
		uint32_t caller_addr = uint32_list_pop_head(addr_queue);
		LIST_HEAD(aq);
		int ret = get_called_func_by_address(caller_addr, &aq);
		if (ret) {
			return ret;
		}

		while (!list_empty(&aq)) {
			uint32_t callee_addr = uint32_list_pop_head(&aq);
			addr_graph_add_call(caller_addr, callee_addr);
			uint32_list_insert_tail(addr_queue, callee_addr);
		}
	}

	addr_graph_fixup();
	return 0;
}

void fini_call_graph()
{
	addr_graph_fini();
}
