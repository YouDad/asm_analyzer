#include "modules/analyzer.h"
#include "modules/addr_graph.h"

int get_callee_by_addr(uint32_t addr, struct uint32_list *callee_addr_list)
{
	LIST_HEAD(iblist);

	int ret = get_func_by_addr(addr, &iblist);
	if (ret) {
		return ret;
	}

	struct instruction_block *item, *tmp;
	list_for_each_entry_safe(item, tmp, &iblist, list) {
		for (struct instruction *i = item->ip; 1; i++) {
			if (strstr(i->string, "bl\t") == i->string) {
				// bl <addr>
				uint32_t new_addr;
				int ret = sscanf(i->string, "%*s%x", &new_addr);
				if (ret != 1) {
					return -EINTERNAL;
				}
				uint32_list_push(callee_addr_list, new_addr);
			}

			if (i->addr >= item->end_addr) {
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

		int ret = get_callee_by_addr(caller_addr, &aq);
		if (ret == -ENOADDR) {
			printf("caller_addr: %x, cannot found\n", caller_addr);
		} else if (ret) {
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

void get_dest_funcs(struct uint32_list *addr_list)
{
	addr_graph_get_dst_node(addr_list);
}

int get_caller_by_addr(uint32_t addr, struct uint32_list *caller_addr_list)
{
	struct addr_edge i;
	int _ret, caller_addr;

	addr_graph_caller_for_each(i, _ret, addr, caller_addr) {
		uint32_list_push(caller_addr_list, caller_addr);
	}
	if (_ret) {
		return _ret;
	}

	return 0;
}
