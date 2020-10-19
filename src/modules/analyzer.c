#include "modules/analyzer.h"
#include "modules/addr_graph.h"

int get_callee_by_addr(uint32_t addr, struct uint32_list *callee_addr_list)
{
	LIST_HEAD(iblist);

	int ret = get_func_by_addr(addr, 0, &iblist);
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

static int _regs_affected_by_addr(uint32_t addr, uint32_t *arg_regs, uint32_t *write_regs)
{
	struct instruction_block ib;
	uint32_list_define(aq);
	int ret = get_inst_block_by_addr_until_jump(addr, &aq, &ib);
	if (ret < 0) {
		return ret;
	}
	if (ret > 0) {
		return 0;
	}

	for (struct instruction *i = ib.ip; 1; i++) {
		uint32_list_define(r);
		uint32_list_define(w);
		ret = regs_affected_by_inst(i, &r, &w);
		if (ret) {
			printf("process failed: \"\033[32m%s\033[0m\"\n", i->string);
		}

		while (!uint32_list_empty(&r)) {
			int x = uint32_list_pop(&r);
			if (!(*write_regs & (1 << x))) {
				*arg_regs |= 1 << x;
			}
		}

		while (!uint32_list_empty(&w)) {
			int x = uint32_list_pop(&w);
			*write_regs |= 1 << x;
		}

		if (i->addr >= ib.end_addr) {
			break;
		}
	}

	uint32_list_define(args);
	uint32_list_define(writes);

	while (!uint32_list_empty(&aq)) {
		int jump_addr = uint32_list_pop(&aq);
		uint32_t arg = *arg_regs;
		uint32_t write = *write_regs;
		struct bitmap now;
		save_visited(&now);
		ret = _regs_affected_by_addr(jump_addr, &arg, &write);
		load_visited(&now);
		if (ret) {
			goto exit;
		}

		uint32_list_push(&args, arg);
		uint32_list_push(&writes, write);
	}

exit:
	while (!uint32_list_empty(&args)) {
		*arg_regs |= uint32_list_pop(&args);
	}
	while (!uint32_list_empty(&writes)) {
		*write_regs |= uint32_list_pop(&writes);
	}
	return 0;
}

int get_arg_regs_by_addr(uint32_t addr, uint32_t *arg_regs)
{
	uint32_t write_regs = 0;
	*arg_regs = 0;
	struct bitmap now;
	save_visited(&now);
	clear_visited();
	int ret = _regs_affected_by_addr(addr, arg_regs, &write_regs);
	load_visited(&now);
	return ret;
}
