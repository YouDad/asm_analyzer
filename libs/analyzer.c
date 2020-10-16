#include "common.h"
#include "types.h"
#include "analyzer.h"

char *str_pool;
vector_define(char *, lines);
static bitmap_define(visited);

int analyzer_load(char *filename)
{
	FILE *fp = fopen(filename, "r");
	if (!fp) {
		return -ENOENT;
	}

	fseek(fp, 0, SEEK_END);
	int fsize = ftell(fp);

	str_pool = MALLOC(char, fsize + 1);
	fseek(fp, 0, SEEK_SET);
	fread(str_pool, fsize, 1, fp);
	fclose(fp);

	vector_init(lines);

	vector_push(lines, str_pool);
	char *p = str_pool;
	while (p < str_pool + fsize) {
		while (*p++ != '\n');
		vector_push(lines, p);
		p[-1] = 0;
	}

	vector_fixup(lines);
	bitmap_init(&visited, vector_size(lines));

	return 0;
}

void analyzer_clean()
{
	vector_fini(lines);
	free(str_pool);
	bitmap_fini(visited);
}

int get_address_by_line(uint32_t line)
{
	char buf[4];
	int address;

	int ret = sscanf(lines[line], "%x%[:]", &address, buf);
	if (ret == 2) {
		return address;
	}
	return -1;
}

int get_line_by_address(uint32_t address)
{
	int l = 0, r = vector_size(lines) - 1;
	while (get_address_by_line(l) < 0) {
		l++;
	}
	while (get_address_by_line(r) < 0) {
		r--;
	}

	int ret;
	while (l <= r) {
		int mid = (l + r)/2;
		int line = get_address_by_line(mid);
		if (line == -1) {
			return -2;
		}

		if (line == address) {
			return mid;
		}

		if (line > address) {
			r = mid - 1;
		} else {
			l = mid + 1;
		}
	}
	return -1;
}

struct instruction get_instruction_by_line(uint32_t line)
{
	struct instruction i;
	char instr[128];
	sscanf(lines[line], "%x:%x\t %[^/]",
			&i.address, &i.code, instr);

	char *p = instr + strlen(instr) - 1;
	while (*p <= ' ') {
		p--;
	}
	p[1] = 0;

	strncpy(i.string, instr, sizeof(i.string));

	return i;
}

/* get_instructions_by_address
 * 返回从start_address开始的连续指令块
 * 并且把遇到的跳转指令存到address_queue中 */
int get_instructions_by_address(uint32_t start_address,
		struct list_head *address_queue,
		struct instructions *retval)
{
	if (start_address % 4) {
		return -EINVAL;
	}

	vector_define(struct instruction, instrs);
	vector_init(instrs);

	int line = get_line_by_address(start_address);
	while (!bitmap_get(visited, line)) {
		struct instruction i = get_instruction_by_line(line);
		vector_push(instrs, i);
		bitmap_set(visited, line++);

		int is_jump_instr = strstr(i.string, "cbnz") == i.string;
		is_jump_instr += strstr(i.string, "cbz") == i.string;
		is_jump_instr += strstr(i.string, "tbnz") == i.string;
		is_jump_instr += strstr(i.string, "tbz") == i.string;

		if (is_jump_instr) {
			// cbnz, cbz, tbnz, tbz
			uint32_t new_address;
			int ret = sscanf(i.string, "%*s%*s%x", &new_address);
			if (ret != 1) {
				return -EINTERNAL;
			}

			uint32_list_insert_tail(address_queue, new_address);
			continue;
		}

		if (i.string[0] == 'b') {
			// br
			if (i.string[1] == 'r') {
				break;
			}

			// b, b.cond
			if (i.string[1] == '\t') {
				uint32_t new_address;
				int ret = sscanf(i.string, "%*s%x", &new_address);
				if (ret != 1) {
					return -EINTERNAL;
				}
				uint32_list_insert_tail(address_queue, new_address);
				break;
			}

			if (i.string[1] == '.') {
				uint32_t new_address;
				int ret = sscanf(i.string, "%*s%x", &new_address);
				if (ret != 1) {
					return -EINTERNAL;
				}
				uint32_list_insert_tail(address_queue, new_address);
				continue;
			}
		}

		is_jump_instr = strstr(i.string, "ret") == i.string;
		is_jump_instr += strstr(i.string, "eret") == i.string;
		if (is_jump_instr) {
			break;
		}
	}

	vector_fixup(instrs);

	retval->ip = instrs;
	retval->start_address = start_address;
	retval->end_address = start_address + vector_size(instrs) * 4 - 4;

	return 0;
}
