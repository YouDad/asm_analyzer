#include "modules/translater.h"
#include "utils/color.h"

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
				*str_cnt += snprintf(&str[*str_cnt], len, "<%db>", i);
			}
		}
		for (int i = _used; i < _cnt; i++) {
			if (_addrs[i] == jump_addr) {
				*str_cnt += snprintf(&str[*str_cnt], len, "<%df>", i);
			}
		}
	}

	return 0;
}

static void _translate_code(const struct instruction *inst, char *str, int *str_cnt, int len)
{
	*str_cnt += snprintf(&str[*str_cnt], len, "\t%x:\t", inst->addr);

#define is_instruction(_inst) \
	else if (strstr(inst->string, _inst) == inst->string)

	if (0);
	is_instruction("cbnz\t") {
		char rt[4], addr[16];
		int ret = sscanf(inst->string, "%*s\t" "%[^,], %s", rt, addr);

		*str_cnt += snprintf(&str[*str_cnt], len,
				_green("if %s != 0 then goto text_0x%s();"), rt, addr);
		return;
	}
	is_instruction("cbz\t") {
		char rt[4], addr[16];
		int ret = sscanf(inst->string, "%*s\t" "%[^,], %s", rt, addr);

		*str_cnt += snprintf(&str[*str_cnt], len,
				_green("if %s == 0 then goto text_0x%s();"), rt, addr);
		return;
	}
	is_instruction("b\t") {
		*str_cnt += snprintf(&str[*str_cnt], len,
				_green("goto "));
		return;
	}
	is_instruction("b.") {
		char op[4], addr[16];
		int ret = sscanf(inst->string, "b.%s\t" "%s", op, addr);

		const char *cond[] = {"eq", "ne", "cs", "cc", "mi",
			"pl", "vs", "vc", "hi", "ls", "ge", "lt", "gt", "le"};
		const char *mean[] = {"==", "!=", "u>=", "u<", "<0", ">=0",
			"overflow", "not overflow", "u>", "u<=", "s>=", "s<", "s>", "s<=", "unknown"};

		int i = 0;
		for (i = 0; i < array_size(cond); i++) {
			if (strcmp(op, cond[i]) == 0) {
				break;
			}
		}

		*str_cnt += snprintf(&str[*str_cnt], len,
				_green("if %s then goto "), mean[i]);
		return;
	}
	is_instruction("bl\t") {
		char addr[16];
		int ret = sscanf(inst->string, "%*s\t" "%s", addr);
		*str_cnt += snprintf(&str[*str_cnt], len,
				_green("text_0x%s();"), addr);
		return;
	}
	is_instruction("sub\t") {
		// rd, rn, rm{, extend {#amount}}
		// rd, rn, imm{, shift}
		// rd, rn, rm{, shift}
		char rd[8], rn[8], rmimm[8], other[16];
		int ret = sscanf(inst->string, "%*s\t"
				"%[^,], %[^,], %[^,]%[^.]",
				rd, rn, rmimm, other);
		if (ret == 3) {
			*str_cnt += snprintf(&str[*str_cnt], len,
					_green("%s = %s - %s;"),
					rd, rn, rmimm[0] == '#' ? &rmimm[1] : rmimm);
			return;
		}

		int shift = 0;
		char extend[8];
		if (strstr(other, "#") != NULL) {
			sscanf(other, ", %s #%d", extend, &shift);
		} else {
			sscanf(other, ", %s", extend);
		}

		if (strcmp(extend, "lsl") == 0) {
			if (rd[0] == 'x') {
				strcpy(extend, "uxtx");
			} else {
				strcpy(extend, "uxtw");
			}
		}

		char types[8];
		switch (extend[3]) {
			case 'b': strcpy(types, "int8_t"); break;
			case 'h': strcpy(types, "int16_t"); break;
			case 'w': strcpy(types, "int32_t"); break;
			case 'x': strcpy(types, "int64_t"); break;
		}

		if (shift) {
			*str_cnt += snprintf(&str[*str_cnt], len,
					_green("%s = %s - (%s%s)(%s << %d);"),
					rd, rn, extend[0] == 'u' ? "u" : "", types, rmimm, shift);
		} else {
			*str_cnt += snprintf(&str[*str_cnt], len,
					_green("%s = %s - (%s%s)%s;"),
					rd, rn, extend[0] == 'u' ? "u" : "", types, rmimm);
		}
		return;
	}
	is_instruction("add\t") {
		// rd, rn, rm{, extend {#amount}}
		// rd, rn, imm{, shift}
		// rd, rn, rm{, shift}
		char rd[8], rn[8], rmimm[8], other[16];
		int ret = sscanf(inst->string, "%*s\t"
				"%[^,], %[^,], %[^,]%[^.]",
				rd, rn, rmimm, other);
		if (ret == 3) {
			*str_cnt += snprintf(&str[*str_cnt], len,
					_green("%s = %s + %s;"),
					rd, rn, rmimm[0] == '#' ? &rmimm[1] : rmimm);
			return;
		}

		int shift = 0;
		char extend[8];
		if (strstr(other, "#") != NULL) {
			sscanf(other, ", %s #%d", extend, &shift);
		} else {
			sscanf(other, ", %s", extend);
		}

		if (strcmp(extend, "lsl") == 0) {
			if (rd[0] == 'x') {
				strcpy(extend, "uxtx");
			} else {
				strcpy(extend, "uxtw");
			}
		}

		char types[8];
		switch (extend[3]) {
			case 'b': strcpy(types, "int8_t"); break;
			case 'h': strcpy(types, "int16_t"); break;
			case 'w': strcpy(types, "int32_t"); break;
			case 'x': strcpy(types, "int64_t"); break;
		}

		if (shift) {
			*str_cnt += snprintf(&str[*str_cnt], len,
					_green("%s = %s + (%s%s)(%s << %d);"),
					rd, rn, extend[0] == 'u' ? "u" : "", types, rmimm, shift);
		} else {
			*str_cnt += snprintf(&str[*str_cnt], len,
					_green("%s = %s + (%s%s)%s;"),
					rd, rn, extend[0] == 'u' ? "u" : "", types, rmimm);
		}
		return;
	}
	is_instruction("ldrb\t") {
		char rt[8], rn[8], other[16];
		int ret, imm;
		bool wback, postindex;

		if (strstr(inst->string, "],") != NULL) {
			// rt, [rn], #imm
			ret = sscanf(inst->string, "%*s\t"
					"%[^,], [%[^]]], #%d", rt, rn, &imm);
			wback = true;
			postindex = true;
		} else if (strstr(inst->string, "]!") != NULL) {
			// rt, [rn, #imm]!
			ret = sscanf(inst->string, "%*s\t"
					"%[^,], [%[^,], #%d]!", rt, rn, &imm);
			wback = true;
			postindex = false;
		} else {
			// rt, [rn{, #imm}]
			ret = sscanf(inst->string, "%*s\t"
					"%[^,], [%[^],]%[^.]", rt, rn, other);
			if (other[0] == ',') {
				sscanf(other, ", #%d", &imm);
			} else {
				imm = 0;
			}

			wback = false;
			postindex = false;
		}

		if (ret != 3) {
			printf("%s\n%d: %s, %s, %d\n", inst->string, ret, rt, rn, imm);
			return;
		}

		bool first = true;
		if (wback && !postindex) {
			if (!first) {
				*str_cnt += snprintf(&str[*str_cnt], len, "\n\t%x:\t", inst->addr);
			}
			first = false;
			*str_cnt += snprintf(&str[*str_cnt], len,
					_green("%s += %d;"), rn, imm);
			imm = 0;
		}
		if (!first) {
			*str_cnt += snprintf(&str[*str_cnt], len, "\n\t%x:\t", inst->addr);
		}
		first = false;
		*str_cnt += snprintf(&str[*str_cnt], len,
				_green("%s = (uint8_t *)%s[%d];"), rt, rn, imm);
		if (wback && postindex) {
			if (!first) {
				*str_cnt += snprintf(&str[*str_cnt], len, "\n\t%x:\t", inst->addr);
			}
			first = false;
			*str_cnt += snprintf(&str[*str_cnt], len,
					_green("%s += %d;"), rn, imm);
		}
		return;
	}
	is_instruction("ldp\t") {
		char rt1[8], rt2[8], rn[8], other[16];
		int ret, imm;
		bool wback, postindex;

		if (strstr(inst->string, "],") != NULL) {
			// rt1, rt2, [rn], #imm
			ret = sscanf(inst->string, "%*s\t"
					"%[^,], %[^,], [%[^]]], #%d", rt1, rt2, rn, &imm);
			wback = true;
			postindex = true;
		} else if (strstr(inst->string, "]!") != NULL) {
			// rt1, rt2, [rn, #imm]!
			ret = sscanf(inst->string, "%*s\t"
					"%[^,], %[^,], [%[^,], #%d]!", rt1, rt2, rn, &imm);
			wback = true;
			postindex = false;
		} else {
			// rt1, rt2, [rn{, #imm}]
			ret = sscanf(inst->string, "%*s\t"
					"%[^,], %[^,], [%[^],]%[^.]", rt1, rt2, rn, other);
			if (other[0] == ',') {
				sscanf(other, ", #%d", &imm);
			} else {
				imm = 0;
			}

			wback = false;
			postindex = false;
		}

		if (ret != 4) {
			printf("%s\n%d: %s, %s, %s, %d\n", inst->string, ret, rt1, rt2, rn, imm);
			return;
		}

		int datasize = 32;
		if (rt1[0] == 'x') {
			datasize = 64;
		}

		bool first = true;
		if (wback && !postindex) {
			if (!first) {
				*str_cnt += snprintf(&str[*str_cnt], len, "\n\t%x:\t", inst->addr);
			}
			first = false;
			*str_cnt += snprintf(&str[*str_cnt], len,
					_green("%s += %d;"), rn, imm);
			imm = 0;
		}
		if (!first) {
			*str_cnt += snprintf(&str[*str_cnt], len, "\n\t%x:\t", inst->addr);
		}
		first = false;
		*str_cnt += snprintf(&str[*str_cnt], len,
				_green("%s = (uint%d_t *)%s[%d];"),
				rt1, datasize, rn, imm * 8 / datasize);
		if (!first) {
			*str_cnt += snprintf(&str[*str_cnt], len, "\n\t%x:\t", inst->addr);
		}
		first = false;
		*str_cnt += snprintf(&str[*str_cnt], len,
				_green("%s = (uint%d_t *)%s[%d];"),
				rt2, datasize, rn, imm * 8 / datasize + 1);
		if (wback && postindex) {
			if (!first) {
				*str_cnt += snprintf(&str[*str_cnt], len, "\n\t%x:\t", inst->addr);
			}
			first = false;
			*str_cnt += snprintf(&str[*str_cnt], len,
					_green("%s += %d;"), rn, imm);
		}
		return;
	}
	is_instruction("stp\t") {
		char rt1[8], rt2[8], rn[8], other[16];
		int ret, imm;
		bool wback, postindex;

		if (strstr(inst->string, "],") != NULL) {
			// rt1, rt2, [rn], #imm
			ret = sscanf(inst->string, "%*s\t"
					"%[^,], %[^,], [%[^]]], #%d", rt1, rt2, rn, &imm);
			wback = true;
			postindex = true;
		} else if (strstr(inst->string, "]!") != NULL) {
			// rt1, rt2, [rn, #imm]!
			ret = sscanf(inst->string, "%*s\t"
					"%[^,], %[^,], [%[^,], #%d]!", rt1, rt2, rn, &imm);
			wback = true;
			postindex = false;
		} else {
			// rt1, rt2, [rn{, #imm}]
			ret = sscanf(inst->string, "%*s\t"
					"%[^,], %[^,], [%[^],]%[^.]", rt1, rt2, rn, other);
			if (other[0] == ',') {
				sscanf(other, ", #%d", &imm);
			} else {
				imm = 0;
			}

			wback = false;
			postindex = false;
		}

		if (ret != 4) {
			printf("%s\n%d: %s, %s, %s, %d\n", inst->string, ret, rt1, rt2, rn, imm);
			return;
		}

		int datasize = 32;
		if (rt1[0] == 'x') {
			datasize = 64;
		}

		bool first = true;
		if (wback && !postindex) {
			if (!first) {
				*str_cnt += snprintf(&str[*str_cnt], len, "\n\t%x:\t", inst->addr);
			}
			first = false;
			*str_cnt += snprintf(&str[*str_cnt], len,
					_green("%s += %d;"), rn, imm);
			imm = 0;
		}
		if (!first) {
			*str_cnt += snprintf(&str[*str_cnt], len, "\n\t%x:\t", inst->addr);
		}
		first = false;
		*str_cnt += snprintf(&str[*str_cnt], len,
				_green("(uint%d_t *)%s[%d] = %s;"),
				datasize, rn, imm * 8 / datasize, rt1);
		if (!first) {
			*str_cnt += snprintf(&str[*str_cnt], len, "\n\t%x:\t", inst->addr);
		}
		first = false;
		*str_cnt += snprintf(&str[*str_cnt], len,
				_green("(uint%d_t *)%s[%d] = %s;"),
				datasize, rn, imm * 8 / datasize + 1, rt2);
		if (wback && postindex) {
			if (!first) {
				*str_cnt += snprintf(&str[*str_cnt], len, "\n\t%x:\t", inst->addr);
			}
			first = false;
			*str_cnt += snprintf(&str[*str_cnt], len,
					_green("%s += %d;"), rn, imm);
		}
		return;
	}
	is_instruction("str\t") {
		char rt[8], rn[8], other[16];
		int ret, imm;
		bool wback, postindex;

		if (strstr(inst->string, "],") != NULL) {
			// rt, [rn], #imm
			ret = sscanf(inst->string, "%*s\t"
					"%[^,], [%[^]]], #%d", rt, rn, &imm);
			wback = true;
			postindex = true;
		} else if (strstr(inst->string, "]!") != NULL) {
			// rt, [rn, #imm]!
			ret = sscanf(inst->string, "%*s\t"
					"%[^,], [%[^,], #%d]!", rt, rn, &imm);
			wback = true;
			postindex = false;
		} else {
			// rt, [rn{, #imm}]
			ret = sscanf(inst->string, "%*s\t"
					"%[^,], [%[^],]%[^.]", rt, rn, other);
			if (other[0] == ',') {
				sscanf(other, ", #%d", &imm);
			} else {
				imm = 0;
			}

			wback = false;
			postindex = false;
		}

		if (ret != 3) {
			printf("%s\n%d: %s, %s, %d\n", inst->string, ret, rt, rn, imm);
			return;
		}

		int datasize = 32;
		if (rt[0] == 'x') {
			datasize = 64;
		}

		bool first = true;
		if (wback && !postindex) {
			if (!first) {
				*str_cnt += snprintf(&str[*str_cnt], len, "\n\t%x:\t", inst->addr);
			}
			first = false;
			*str_cnt += snprintf(&str[*str_cnt], len,
					_green("%s += %d;"), rn, imm);
			imm = 0;
		}
		if (!first) {
			*str_cnt += snprintf(&str[*str_cnt], len, "\n\t%x:\t", inst->addr);
		}
		first = false;
		*str_cnt += snprintf(&str[*str_cnt], len,
				_green("(uint%d_t *)%s[%d] = %s;"),
				datasize, rn, imm * 8 / datasize, rt);
		if (wback && postindex) {
			if (!first) {
				*str_cnt += snprintf(&str[*str_cnt], len, "\n\t%x:\t", inst->addr);
			}
			first = false;
			*str_cnt += snprintf(&str[*str_cnt], len,
					_green("%s += %d;"), rn, imm);
		}
		return;
	}
	is_instruction("msr\t") {
		// xd, xn/imm
		char xd[24], xn[8];
		sscanf(inst->string, "%*s\t%[^,], %s", xd, xn);
		*str_cnt += snprintf(&str[*str_cnt], len,
				_green("%s = %s;"), xd, xn[0] == '#' ? &xn[1] : xn);
		return;
	}
	is_instruction("mrs\t") {
		// xd, xn/imm
		char xd[8], xn[24];
		sscanf(inst->string, "%*s\t%[^,], %s", xd, xn);
		*str_cnt += snprintf(&str[*str_cnt], len,
				_green("%s = %s;"), xd, xn[0] == '#' ? &xn[1] : xn);
		return;
	}
	is_instruction("movk\t") {
		// rd, #imm{, lsl #shift}
		char xd[8], xn[8], other[16];
		int ret = sscanf(inst->string, "%*s\t%[^,], #%[0-9xa-f]%[^.]", xd, xn, other);
		if (ret == 2) {
			*str_cnt += snprintf(&str[*str_cnt], len,
					_green("%s = %s;"), xd, xn[0] == '#' ? &xn[1] : xn);
			return;
		} else {
			int shift;
			ret = sscanf(other, ", lsl #%d", &shift);
			if (ret == 1) {
				*str_cnt += snprintf(&str[*str_cnt], len,
						_green("%s = (%s & 0x%x) | (%s << %d);"),
						xd, xd, 0xffff << shift, xn, shift);
				return;
			}
		}
	}
	is_instruction("mov\t") {
		// xd, xn/imm
		char xd[8], xn[8];
		sscanf(inst->string, "%*s\t%[^,], %s", xd, xn);
		*str_cnt += snprintf(&str[*str_cnt], len,
				_green("%s = %s;"), xd, xn[0] == '#' ? &xn[1] : xn);
		return;
	}
	is_instruction("adr\t") {
		// xd, xn/imm
		char xd[8], xn[8];
		sscanf(inst->string, "%*s\t%[^,], %s", xd, xn);
		*str_cnt += snprintf(&str[*str_cnt], len,
				_green("%s = %s;"), xd, xn[0] == '#' ? &xn[1] : xn);
		return;
	}
	is_instruction("adrp\t") {
		// xd, xn/imm
		char xd[8], xn[8];
		sscanf(inst->string, "%*s\t%[^,], %s", xd, xn);
		*str_cnt += snprintf(&str[*str_cnt], len,
				_green("%s = %s;"), xd, xn[0] == '#' ? &xn[1] : xn);
		return;
	}
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

		for (int i = 0; i < jump_addr_cnt; i++) {
			for (int j = 0; j < i + 1; j++) {
				if (jump_addrs[i] < jump_addrs[j]) {
					uint32_t tmp = jump_addrs[i];
					jump_addrs[i] = jump_addrs[j];
					jump_addrs[j] = tmp;
				}
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
