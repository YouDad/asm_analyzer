#include "types/map.h"

map_define(int, int, int_int_map);

int main()
{
	map_init(int_int_map);
	map_set(int_int_map, 1, 2);
	int ret, have;
	have = map_get(int_int_map, 1, &ret);
	if (have != 1 || ret != 2) {
		return 1;
	}

	have = map_get(int_int_map, 2, &ret);
	if (have != 0) {
		return 2;
	}
	map_fini(int_int_map);
	return 0;
}
