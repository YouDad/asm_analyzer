#include "types/graph.h"
#include <stdio.h>

int main()
{
	struct graph g;
	graph_init(&g);

	graph_addedge(&g, 1, 2);
	graph_addedge(&g, 1, 3);
	graph_addedge(&g, 1, 4);
	graph_addedge(&g, 1, 7);

	graph_addedge(&g, 2, 3);

	graph_addedge(&g, 3, 4);

	graph_fixup(&g);

	int sum = 0;
	graph_for_each(g, 1) {
		sum += i->dest;
	}
	if (sum != 2+3+4+7) {
		return 1;
	}

	sum = 0;
	graph_for_each(g, 2) {
		sum += i->dest;
	}
	if (sum != 3) {
		return 2;
	}

	sum = 0;
	graph_for_each(g, 3) {
		sum += i->dest;
	}
	if (sum != 4) {
		return 3;
	}

	graph_fini(&g);
	return 0;
}
