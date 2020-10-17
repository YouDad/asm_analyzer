#pragma once
#include "common.h"
#include "types/map.h"
#include "types/graph.h"

map_define(int, int, addr2node);
map_define(int, int, node2addr);
struct graph callee; // caller -> callee
struct graph caller; // callee -> caller
int node_cnt;

static inline void addr_graph_init()
{
	map_init(addr2node);
	map_init(node2addr);
	graph_init(&callee);
	graph_init(&caller);
	node_cnt = 0;
}

static inline void addr_graph_fini()
{
	map_fini(addr2node);
	map_fini(node2addr);
	graph_fini(&callee);
	graph_fini(&caller);
}

static inline int _addr_graph_get_nodeid(int addr)
{
	int nodeid;
	int have = map_get(addr2node, addr, &nodeid);
	if (have) {
		return nodeid;
	}
	map_set(addr2node, addr, node_cnt);
	map_set(node2addr, node_cnt, addr);
	return node_cnt++;
}

static inline int addr_graph_add_call(int caller_addr, int callee_addr)
{
	int callee_nodeid = _addr_graph_get_nodeid(callee_addr);
	int caller_nodeid = _addr_graph_get_nodeid(caller_addr);
	int ret = graph_addedge(&callee, caller_nodeid, callee_nodeid);
	if (ret) {
		return ret;
	}

	ret = graph_addedge(&caller, callee_nodeid, caller_nodeid);
	return ret;
}

static inline int addr_graph_fixup()
{
	int ret = graph_fixup(&callee);
	if (ret) {
		return ret;
	}

	ret = graph_fixup(&caller);
	return ret;
}

struct addr_edge {
	struct edge *e;
	int addr;
};

static inline int addr_edge_is_valid(struct addr_edge *ae)
{
	return ae->e != NULL;
}

static inline int addr_edge_get_addr(struct addr_edge *ae)
{
	return ae->addr;
}

/* struct addr_edge *i; // for iter
 * int _ret; // function ret, should process it after for
 * int addr; // param
 * int callee_addr; // got callee addr
 * */
#define addr_graph_callee_for_each(i, _ret, addr, callee_addr)                           \
	for (_ret = addr_graph_callee_first(addr, &i), callee_addr = addr_edge_get_addr(&i); \
			_ret == 0 && addr_edge_is_valid(&i);                                         \
			_ret = addr_graph_callee_next(&i), callee_addr = addr_edge_get_addr(&i))

static inline int addr_graph_callee_first(int addr, struct addr_edge *ae)
{
	int nodeid = _addr_graph_get_nodeid(addr);

	ae->e = graph_first(&callee, nodeid);
	if (!ae->e) {
		return 0;
	}

	int ret = map_get(node2addr, ae->e->dest, &ae->addr);
	if (!ret) {
		return -EINTERNAL;
	}

	return 0;
}

static inline int addr_graph_callee_next(struct addr_edge *ae)
{
	ae->e = graph_next(&callee, ae->e);
	if (!ae->e) {
		return 0;
	}

	int ret = map_get(node2addr, ae->e->dest, &ae->addr);
	if (!ret) {
		return -EINTERNAL;
	}

	return 0;
}

/* struct addr_edge *i; // for iter
 * int _ret; // function ret, should process it after for
 * int addr; // param
 * int caller_addr; // got caller addr
 * */
#define addr_graph_caller_for_each(i, _ret, addr, caller_addr)                           \
	for (_ret = addr_graph_caller_first(addr, &i), caller_addr = addr_edge_get_addr(&i); \
			_ret == 0 && addr_edge_is_valid(&i);                                         \
			_ret = addr_graph_caller_next(&i), caller_addr = addr_edge_get_addr(&i))

static inline int addr_graph_caller_first(int addr, struct addr_edge *ae)
{
	int nodeid = _addr_graph_get_nodeid(addr);

	ae->e = graph_first(&caller, nodeid);
	if (!ae->e) {
		return 0;
	}

	int ret = map_get(node2addr, ae->e->dest, &ae->addr);
	if (!ret) {
		return -EINTERNAL;
	}

	return 0;
}

static inline int addr_graph_caller_next(struct addr_edge *ae)
{
	ae->e = graph_next(&caller, ae->e);
	if (!ae->e) {
		return 0;
	}

	int ret = map_get(node2addr, ae->e->dest, &ae->addr);
	if (!ret) {
		return -EINTERNAL;
	}

	return 0;
}
