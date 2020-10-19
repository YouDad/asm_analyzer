#include "modules/decoder.h"

int get_callee_by_addr(uint32_t addr, struct uint32_list *callee_addr_list);
int init_call_graph(struct uint32_list *addr_queue);
void fini_call_graph();
void get_dest_funcs(struct uint32_list *addr_list);
int get_caller_by_addr(uint32_t addr, struct uint32_list *caller_addr_list);
