#include "modules/translater.h"
#include "utils/color.h"
#include "insts/armv8.h"

static int _add_tag(char *inst_str,
		uint32_t *_addrs, int _used, int _cnt,
		char *str, int *str_cnt, int len)
{
	uint32_t jump_addr;
	int ret = get_jump_addr_by_str(inst_str, &jump_addr);
	if (ret < 0) {
		return ret;
	}
	if (ret == 0 && _addrs) {
		for (int i = 0; i < _used; i++) {
			if (_addrs[i] == jump_addr) {
				*str_cnt += snprintf(&str[*str_cnt], len, "%db; // <%db, %x>", i, i, jump_addr);
			}
		}
		for (int i = _used; i < _cnt; i++) {
			if (_addrs[i] == jump_addr) {
				*str_cnt += snprintf(&str[*str_cnt], len, "%df; // <%df, %x>", i, i, jump_addr);
			}
		}
	}

	return 0;
}

static void _translate_code(const struct instruction *inst, char *str, int *str_cnt, int len)
{
	int ret;
	for (int i = 0; i < array_size(armv8_insts); i++) {
		if (strstr(inst->string, armv8_insts[i].inst) == inst->string) {
			ret = armv8_insts[i].translate(inst, str, str_cnt, len);
			if (ret) {
				printf("%s failed\n", inst->string);
			}
			return;
		}
	}

	*str_cnt += snprintf(&str[*str_cnt], len, "\t%x:\t", inst->addr);
	const char *p = inst->string;
	while (*p) {
		if (p[0] == '<') {
			break;
		}
		str[(*str_cnt)++] = *(p++);
	}
}

int translate(struct list_head *iblist, char *str, int len)
{
	int str_cnt = 0;
	struct instruction_block *ib, *tmp;

	int jump_addr_cnt = 0;
	uint32_list_define(jump_addr_list);
	list_for_each_entry_safe(ib, tmp, iblist, list) {
		int ibsize = (ib->end_addr - ib->start_addr + 4) / 4;
		for (int i = 0; i < ibsize; i++) {
			uint32_t new_addr;
			int ret = get_jump_addr_by_str(ib->ip[i].string, &new_addr);
			if (ret < 0) {
				return ret;
			}
			if (ret == 0) {
				uint32_list_push(&jump_addr_list, new_addr);
				jump_addr_cnt++;
			}
		}
	}

	uint32_t *jump_addrs = NULL;
	if (jump_addr_cnt) {
		jump_addrs = MALLOC(uint32_t, jump_addr_cnt);
		jump_addr_cnt = 0;
		uint32_list_define(sorted_addrs);
		while (!uint32_list_empty(&jump_addr_list)) {
			jump_addrs[jump_addr_cnt++] = uint32_list_pop(&jump_addr_list);
		}

		// sort
		for (int i = 0; i < jump_addr_cnt; i++) {
			for (int j = 0; j < i + 1; j++) {
				if (jump_addrs[i] < jump_addrs[j]) {
					uint32_t tmp = jump_addrs[i];
					jump_addrs[i] = jump_addrs[j];
					jump_addrs[j] = tmp;
				}
			}
		}

		// uniq
		for (int i = 0; i < jump_addr_cnt - 1; i++) {
			if (jump_addrs[i] == jump_addrs[i+1]) {
				for (int j = i+1; j < jump_addr_cnt - 1; j++) {
					jump_addrs[j] = jump_addrs[j+1];
				}
				i--;
				jump_addr_cnt--;
			}
		}
	}

	int used_cnt = 0;

	list_for_each_entry_safe(ib, tmp, iblist, list) {
		int ibsize = (ib->end_addr - ib->start_addr + 4) / 4;
		for (int i = 0; i < ibsize; i++) {
			// 0:
			if (jump_addrs && ib->start_addr + i * 4 == jump_addrs[used_cnt]) {
				str_cnt += snprintf(&str[str_cnt], len, "%d:\n", used_cnt++);
			}

			_translate_code(&ib->ip[i], str, &str_cnt, len);

			_add_tag(ib->ip[i].string, jump_addrs,
					used_cnt, jump_addr_cnt, str, &str_cnt, len);

			// \n
			str[str_cnt++] = '\n';
			str[str_cnt] = 0;
		}
		list_del(&ib->list);
		release_instruction_block(ib);
	}
	if (jump_addrs) {
		free(jump_addrs);
	}
	return 0;
}
