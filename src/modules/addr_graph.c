#include "modules/addr_graph.h"

map_define(int, int, addr2node);
map_define(int, int, node2addr);
struct graph callee; // caller -> callee
struct graph caller; // callee -> caller
vector_struct_define(int, in);
vector_struct_define(int, out);
uint32_list_define(src_node);
uint32_list_define(dst_node);
int node_cnt;

void addr_graph_init()
{
	map_init(addr2node);
	map_init(node2addr);
	graph_init(&callee);
	graph_init(&caller);
	vector_init(in);
	vector_init(out);
	node_cnt = 0;
}

void addr_graph_fini()
{
	map_fini(addr2node);
	map_fini(node2addr);
	graph_fini(&callee);
	graph_fini(&caller);
	vector_fini(in);
	vector_fini(out);
}

int _addr_graph_get_nodeid(int addr)
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

int addr_graph_add_call(int caller_addr, int callee_addr)
{
	int callee_nodeid = _addr_graph_get_nodeid(callee_addr);
	int caller_nodeid = _addr_graph_get_nodeid(caller_addr);

	vector_expand(in, callee_nodeid, 0);
	vector_expand(out, caller_nodeid, 0);
	in[callee_nodeid]++;
	out[caller_nodeid]++;

	int ret = graph_addedge(&callee, caller_nodeid, callee_nodeid);
	if (ret) {
		return ret;
	}

	ret = graph_addedge(&caller, callee_nodeid, caller_nodeid);
	return ret;
}

int addr_graph_fixup()
{
	int ret = graph_fixup(&callee);
	if (ret) {
		return ret;
	}

	ret = graph_fixup(&caller);
	if (ret) {
		return ret;
	}

	vector_fixup(in);
	vector_fixup(out);

	for (int i = 0; i < vector_size(in); i++) {
		if (in[i] == 0) {
			uint32_list_push(&src_node, i);
		}
	}

	for (int i = 0; i < vector_size(out); i++) {
		if (out[i] == 0) {
			uint32_list_push(&dst_node, i);
		}
	}

	return ret;
}

int addr_graph_callee_first(int addr, struct addr_edge *ae)
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

int addr_graph_callee_next(struct addr_edge *ae)
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

int addr_graph_caller_first(int addr, struct addr_edge *ae)
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

int addr_graph_caller_next(struct addr_edge *ae)
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
