#include "libs/analyzer.h"

int get_called_func_by_address(uint32_t address,
		struct list_head *called_address_list)
{
	LIST_HEAD(iblist);

	int ret = get_function_by_address(address, &iblist);
	if (ret) {
		return ret;
	}

	struct instruction_block *item, *tmp;
	list_for_each_entry_safe(item, tmp, &iblist, list) {
		struct instruction *i = item->ip;

		while (1) {
			if (strstr(i->string, "bl") == i->string) {
				// bl <addr>
				uint32_t new_address;
				int ret = sscanf(i->string, "%*s%x", &new_address);
				if (ret != 1) {
					return -EINTERNAL;
				}
				uint32_list_insert_tail(called_address_list, new_address);
			}
			if (i->address < item->end_address) {
				i++;
			} else {
				break;
			}
		}
	}

	return 0;
}
