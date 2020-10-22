#include "common.h"
#include "modules/analyzer.h"
#include "modules/translater.h"
#include "handlers.h"

int handle_help(char *args);
int handle_load(char *args);
int handle_exit(char *args);
int handle_list(char *args);
int handle_list_short(char *args);
int handle_add_src(char *args);
int handle_init_analyzer(char *args);
int handle_get_callee(char *args);
int handle_get_caller(char *args);
int handle_get_leaves(char *args);
int handle_func_args(char *args);
int handle_translate(char *args);

struct route routes[] = {
	{"help", "h", handle_help, "outputs help information"},
	{"load", "l", handle_load, "load file to analysis"},
	{"exit", "q", handle_exit, "exit"},
	{"list", "ll", handle_list, "output address code"},
	{"listshort", "ls", handle_list_short, "output short code"},
	{"add_src", "as", handle_add_src, "add src in analyzer"},
	{"get_callee", "ge", handle_get_callee, "get callee of function of addr"},
	{"get_caller", "gr", handle_get_caller, "get caller of function of addr"},
	{"get_leaves", "gv", handle_get_leaves, "get leaf function"},
	{"arg", "a", handle_func_args, "analysis function's arguments"},
	{"translate", "t", handle_translate, "translate asm code"},
};

int dispatch(char cmd[128])
{
	char keyword[128];
	char *args;

	sscanf(cmd, "%s", keyword);
	args = cmd + strlen(keyword) + 1;

	for (int i = 0; i < array_size(routes); i++) {
		struct route *p = routes + i;
		if (p->id[0] && strcmp(p->id, keyword) == 0) {
			return p->handler(args);
		}
		if (p->small_id[0] && strcmp(p->small_id, keyword) == 0) {
			return p->handler(args);
		}
	}

	return handle_help(args);
}

int handle_help(char *args)
{
	for (int i = 0; i < array_size(routes); i++) {
		printf("%s/%s: %s\n", routes[i].small_id, routes[i].id, routes[i].desc);
		if (routes[i].desc2[0]) {
			// TODO: some spaces
			printf("%s\n", routes[i].desc2);
		}
	}
	return 0;
}

bool is_loaded = false;
uint32_list_define(src);
bool is_init_analyzer = false;

int handle_load(char *args)
{
	char filename[128];
	sscanf(args, "%s", filename);

	if (is_loaded) {
		decoder_unload();
	}

	int ret = decoder_load(filename);
	if (ret) {
		perror(filename);
		return ret;
	}

	printf("load file: %s\n", filename);
	is_loaded = true;
	return ret;
}

int handle_exit(char *args)
{
	if (is_loaded) {
		decoder_unload();
	}
	if (is_init_analyzer) {
		fini_call_graph();
	}
	exit(0);
	return 0;
}

int handle_list(char *args)
{
	uint32_t addr;
	sscanf(args, "%x", &addr);

	LIST_HEAD(i10s_list);
	struct instruction_block *item, *tmp;

	int ret = get_func_by_addr(addr, 1, &i10s_list);
	if (ret) {
		printf("get_func_by_addr failed(%d)\n", ret);
		return 1;
	}

	list_for_each_entry_safe(item, tmp, &i10s_list, list) {
		print_instruction_block(item);
		list_del(&item->list);
		release_instruction_block(item);
	}

	return 0;
}

int handle_list_short(char *args)
{
	uint32_t addr;
	sscanf(args, "%x", &addr);

	LIST_HEAD(i10s_list);
	struct instruction_block *item, *tmp;

	int ret = get_func_by_addr(addr, 1, &i10s_list);
	if (ret) {
		printf("get_func_by_addr failed(%d)\n", ret);
		return 1;
	}

	int i = 0;
	list_for_each_entry_safe(item, tmp, &i10s_list, list) {
		print_instruction_block_short(item);
		list_del(&item->list);
		release_instruction_block(item);
		i++;
	}

	return 0;
}

int handle_add_src(char *args)
{
	uint32_t addr;
	sscanf(args, "%x", &addr);
	uint32_list_push(&src, addr);
	return 0;
}

int handle_init_analyzer(char *args)
{
	if (!is_init_analyzer) {
		uint32_list_push(&src, 0);

		int ret = init_call_graph(&src);
		if (ret) {
			return ret;
		}

		is_init_analyzer = true;
		clear_visited();
	}
	return 0;
}

int handle_get_callee(char *args)
{
	handle_init_analyzer(args);
	uint32_t addr;
	sscanf(args, "%x", &addr);

	uint32_list_define(callee_addr_list);
	int ret = get_callee_by_addr(addr, &callee_addr_list);
	if (ret) {
		return ret;
	}

	while (!uint32_list_empty(&callee_addr_list)) {
		printf("%x\n", uint32_list_pop(&callee_addr_list));
	}

	return 0;
}

int handle_get_caller(char *args)
{
	handle_init_analyzer(args);
	uint32_t addr;
	sscanf(args, "%x", &addr);

	uint32_list_define(caller_addr_list);
	int ret = get_caller_by_addr(addr, &caller_addr_list);
	if (ret == -ENOADDR) {
		printf("no such function at addr %x\n", addr);
		return 0;
	} else if (ret) {
		return ret;
	}

	while (!uint32_list_empty(&caller_addr_list)) {
		printf("%x\n", uint32_list_pop(&caller_addr_list));
	}

	return 0;
}

int handle_get_leaves(char *args)
{
	handle_init_analyzer(args);
	uint32_list_define(al);
	get_dest_funcs(&al);
	while (!uint32_list_empty(&al)) {
		uint32_t addr = uint32_list_pop(&al);
		printf("func: %04x\n", addr);
	}
	return 0;
}

int handle_func_args(char *args)
{
	uint32_t addr;
	sscanf(args, "%x", &addr);
	handle_init_analyzer(args);

	uint32_t regs;
	int ret = get_arg_regs_by_addr(addr, &regs);
	if (ret) {
		return ret;
	}

	for (int i = 0 ; i < 32; i++) {
		if (regs & (1 << i)) {
			printf("x%d\n", i);
		}
	}

	return 0;
}

char translate_str[1<<16];

int handle_translate(char *args)
{
	handle_init_analyzer(args);
	uint32_t addr;
	sscanf(args, "%x", &addr);

	LIST_HEAD(iblist);
	int ret = get_func_by_addr(addr, 1, &iblist);
	if (ret) {
		printf("get_func_by_addr failed(%d)\n", ret);
		return 1;
	}

	int len = 0;
	struct instruction_block *ib, *tmp;
	list_for_each_entry_safe(ib, tmp, &iblist, list) {
		len += ib->end_addr - ib->start_addr + 4;
	}
	len *= 10;
	translate(&iblist, translate_str, len);
	printf("%s", translate_str);

	return 0;
}
