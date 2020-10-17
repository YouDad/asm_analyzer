#pragma once
#include "types/vector.h"

/* int graph 整型单向图
 * graph_init(g)          | 初始化图结构g
 * graph_addedge(g, u, v) | 给g加一条u指向v的边
 * graph_fixup(g)         | 固定图结构中的向量，节省内存
 * graph_first(g, u)      | 获取节点u的第一条边
 * graph_next(g, e)       | 获取边的后继边
 * graph_fini(g)          | 释放图结构所占用的内存
 *
 * 初始化流程是：define -> init -> addedge... -> [fixup]
 * 使用的方法是：
 * for (struct edge *i = graph_first(g, u); i;
 *     i = graph_next(g, i)) {
 *     printf("%d\n", i->dest);
 * }
 * 注意的是，循环结束的条件是空指针边
 * u是访问的点的编号
 * 这个循环中的i->dest是这个点u所有边指向的节点编号
 * */

#define graph_for_each(g, u) \
	for (struct edge *i = graph_first(&g, u); i; i = graph_next(&g, i))

struct edge {
	int dest;
	int _next;
};

struct graph {
	vector_struct_define(struct edge, edges);
	vector_struct_define(int, head);
};

static inline int _check_head(struct graph *g, int x)
{
	x++;
	if (x > vector_size(g->head)) {
		vector_pushs(g->head, x - vector_size(g->head), -1);
	}
	return 0;
}

static inline void graph_init(struct graph *g)
{
	vector_struct_init(g->edges);
	vector_struct_init(g->head);
}

static inline int graph_addedge(struct graph *g, int u, int v)
{
	int ret = _check_head(g, u);
	if (ret) return ret;

	ret = _check_head(g, v);
	if (ret) return ret;

	struct edge e = { v, g->head[u] };
	g->head[u] = vector_size(g->edges);
	vector_push(g->edges, e);
	return 0;
}

static inline int graph_fixup(struct graph *g)
{
	vector_fixup(g->edges);
	vector_fixup(g->head);
	return 0;
}

static inline void graph_fini(struct graph *g)
{
	vector_fini(g->edges);
	vector_fini(g->head);
}

static inline struct edge *graph_first(struct graph *g, int u)
{
	if (g->head[u] == -1) {
		return NULL;
	}
	return &g->edges[g->head[u]];
}

static inline struct edge *graph_next(struct graph *g, struct edge *e)
{
	if (e->_next == -1) {
		return NULL;
	}
	return &g->edges[e->_next];
}
