#include "common.h"
#include "libs/decoder.h"

char *str_pool;
vector_define(char *, lines);
static struct bitmap visited;

int decoder_load(char *filename)
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
	bitmap_init(vector_size(lines), &visited);

	return 0;
}

void analyzer_clean()
{
	vector_fini(lines);
	free(str_pool);
	bitmap_fini(&visited);
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

/* get_instruction_block_by_address
 * 返回从start_address开始的连续指令块
 * 并且把遇到的跳转指令存到address_queue中 */
/* retval < 0: error code
 * retval = 0: success
 * retval > 0: no error, but failed */
int get_instruction_block_by_address(uint32_t start_address,
		struct list_head *address_queue,
		struct instruction_block *retval)
{
	if (start_address % 4) {
		return -EINVAL;
	}

	vector_define(struct instruction, instrs);
	vector_init(instrs);

	int line = get_line_by_address(start_address);
	while (!bitmap_get(&visited, line)) {
		struct instruction i = get_instruction_by_line(line);
		int is_jump_instr;
		vector_push(instrs, i);
		bitmap_set(&visited, line++);

		is_jump_instr = strstr(i.string, "cbnz") == i.string;
		is_jump_instr += strstr(i.string, "cbz") == i.string;

		if (is_jump_instr) {
			// cbnz, cbz
			uint32_t new_address;
			int ret = sscanf(i.string, "%*s%*s%x", &new_address);
			if (ret != 1) {
				printf("get new_address failed: %x %s", i.address, i.string);
				return -EINTERNAL;
			}

			uint32_list_insert_tail(address_queue, new_address);
			continue;
		}

		is_jump_instr = strstr(i.string, "tbnz") == i.string;
		is_jump_instr += strstr(i.string, "tbz") == i.string;

		if (is_jump_instr) {
			// tbnz, tbz
			uint32_t new_address;
			int ret = sscanf(i.string, "%*s%*s%*s%x", &new_address);
			if (ret != 1) {
				printf("get new_address failed: %x %s", i.address, i.string);
				return -EINTERNAL;
			}

			uint32_list_insert_tail(address_queue, new_address);
			continue;
		}

		is_jump_instr = strstr(i.string, "ret") == i.string;
		is_jump_instr += strstr(i.string, "eret") == i.string;
		if (is_jump_instr) {
			break;
		}

		if (i.string[0] == 'b') {
			// br
			if (i.string[1] == 'r') {
				break;
			}

			// b, b.cond
			if (i.string[1] == '\t' || i.string[1] == '.') {
				uint32_t new_address;
				int ret = sscanf(i.string, "%*s%x", &new_address);
				if (ret != 1) {
					printf("get new_address failed: %x %s", i.address, i.string);
					return -EINTERNAL;
				}
				uint32_list_insert_tail(address_queue, new_address);

				if (i.string[1] == '\t') {
					break;
				} else {
					continue;
				}
			}
		}
	}

	vector_fixup(instrs);

	if (vector_size(instrs)) {
		retval->ip = instrs;
		retval->start_address = start_address;
		retval->end_address = start_address + vector_size(instrs) * 4 - 4;
		return 0;
	} else {
		return 1;
	}

	return 0;
}

int get_function_by_address(uint32_t start_address,
		struct list_head *instruction_block_list)
{
	if (start_address % 4) {
		return -EINVAL;
	}

	int ret = 0;
	LIST_HEAD(queue);
	LIST_HEAD(tmp_i10s_list);
	uint32_list_insert_tail(&queue, start_address);
	bitmap_clear(&visited);

	while (!list_empty(&queue)) {
		struct instruction_block *i10s = MALLOC(struct instruction_block, 1);
		int addr = uint32_list_pop_head(&queue);

		ret = get_instruction_block_by_address(addr, &queue, i10s);
		if (ret < 0) {
			free(i10s);
			return ret;
		}

		if (ret > 0) {
			free(i10s);
			continue;
		}

		INIT_LIST_HEAD(&i10s->list);
		list_insert_tail(&i10s->list, &tmp_i10s_list);
	}

	while (!list_empty(&tmp_i10s_list)) {
		struct instruction_block *item, *tmp, *min_item;
		uint32_t min_address = 0xffffffff;

		list_for_each_entry_safe(item, tmp, &tmp_i10s_list, list) {
			if (item->start_address < min_address) {
				min_address = item->start_address;
				min_item = item;
			}
		}

		list_del(&min_item->list);

		item = list_last_entry(instruction_block_list, struct instruction_block, list);
		if (can_merge_instruction_block(item, min_item)) {
			merge_instruction_block(item, min_item);
			release_instruction_block(min_item);
		} else {
			list_insert_tail(&min_item->list, instruction_block_list);
		}
	}

	return 0;
}
