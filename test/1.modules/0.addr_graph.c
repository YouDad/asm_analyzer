#include "modules/addr_graph.h"
#include <stdio.h>

int test(int addr, int assert_callee_addr_sum, int assert_caller_addr_sum)
{
	int sum, _ret, callee_addr, caller_addr;
	struct addr_edge i;

	sum = 0;
	addr_graph_callee_for_each(i, _ret, addr, callee_addr) {
		sum += callee_addr;
	}
	if (_ret && _ret != -ENOADDR) {
		printf("callee_for_each failed(%d)\n", _ret);
		printf("%x\n", addr);
		return 1;
	}
	if (sum != assert_callee_addr_sum) {
		printf("callee addr sum assert failed(%x, %x)\n", assert_callee_addr_sum, sum);
		return 2;
	}

	sum = 0;
	addr_graph_caller_for_each(i, _ret, addr, caller_addr) {
		sum += caller_addr;
	}
	if (_ret && _ret != -ENOADDR) {
		printf("caller_for_each failed(%d)\n", _ret);
		printf("%x\n", addr);
		return 3;
	}
	if (sum != assert_caller_addr_sum) {
		printf("caller addr sum assert failed(%x, %x)\n", assert_caller_addr_sum, sum);
		return 4;
	}

	return 0;
}

int main()
{
	int ret;
	addr_graph_init();

	addr_graph_add_call(0x30, 0x50);
	addr_graph_add_call(0x30, 0x60);
	addr_graph_add_call(0x30, 0x19123);
	addr_graph_add_call(0x19123, 0x172346);
	addr_graph_add_call(0x19123, 0x60);
	addr_graph_add_call(0x19123, 0x3467);
	addr_graph_add_call(0x60, 0x19123);
	addr_graph_add_call(0x60, 0x172346);
	addr_graph_add_call(0x60, 0x50);
	addr_graph_fixup();

	ret = test(0x30, 0x50+0x60+0x19123, 0);
	if (ret) return ret;
	ret = test(0x50, 0, 0x30+0x60);
	if (ret) return ret;
	ret = test(0x60, 0x19123+0x172346+0x50, 0x30+0x19123);
	if (ret) return ret;
	ret = test(0x19123, 0x172346+0x60+0x3467, 0x30+0x60);
	if (ret) return ret;
	ret = test(0x172346, 0, 0x19123+0x60);
	if (ret) return ret;
	ret = test(0x3467, 0, 0x19123);
	if (ret) return ret;

	addr_graph_fini();
	return 0;
}
