#pragma once
#include "common.h"
#include "types/map.h"
#include "types/graph.h"
#include "types/uint32_list.h"

struct addr_edge {
	struct edge *e;
	uint32_t addr;
};

static inline int addr_edge_is_valid(struct addr_edge *ae) { return ae->e != NULL; }
static inline uint32_t addr_edge_get_addr(struct addr_edge *ae) { return ae->addr; }

void addr_graph_init();
void addr_graph_fini();
int addr_graph_add_call(uint32_t caller_addr, uint32_t callee_addr);
int addr_graph_fixup();
void addr_graph_get_dst_node(struct uint32_list *list);

/* 遍历addr地址的函数所调用的函数
 * i,_ret,callee_addr需要提前定义
 * addr是参数调用者的地址，readonly
 *
 * for循环中使用callee_addr来获得调用的函数地址
 *
 * struct addr_edge i; // for iter
 * int _ret; // function ret, should process it after for
 * int addr; // param
 * int callee_addr; // got callee addr
 * */
#define addr_graph_callee_for_each(i, _ret, addr, callee_addr)                           \
	for (_ret = addr_graph_callee_first(addr, &i), callee_addr = addr_edge_get_addr(&i); \
			_ret == 0 && addr_edge_is_valid(&i);                                         \
			_ret = addr_graph_callee_next(&i), callee_addr = addr_edge_get_addr(&i))

int addr_graph_callee_first(uint32_t addr, struct addr_edge *ae);
int addr_graph_callee_next(struct addr_edge *ae);

/* 遍历调用addr的函数
 * i,_ret,caller_addr需要提前定义
 * addr是参数被调用者的地址，readonly
 *
 * for循环中使用caller_addr来获得调用addr的函数地址
 *
 * struct addr_edge i; // for iter
 * int _ret; // function ret, should process it after for
 * int addr; // param
 * int caller_addr; // got caller addr
 * */
#define addr_graph_caller_for_each(i, _ret, addr, caller_addr)                           \
	for (_ret = addr_graph_caller_first(addr, &i), caller_addr = addr_edge_get_addr(&i); \
			_ret == 0 && addr_edge_is_valid(&i);                                         \
			_ret = addr_graph_caller_next(&i), caller_addr = addr_edge_get_addr(&i))

int addr_graph_caller_first(uint32_t addr, struct addr_edge *ae);
int addr_graph_caller_next(struct addr_edge *ae);
