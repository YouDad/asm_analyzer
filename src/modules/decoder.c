#include "modules/decoder.h"

char *str_pool;
vector_define(char *, lines);
static struct bitmap visited;

int decoder_load(char *filename)
{
	FILE *fp = fopen(filename, "r");
	if (!fp) {
		error(-ENOENT, "open failed");
	}

	fseek(fp, 0, SEEK_END);
	int fsize = ftell(fp);

	str_pool = MALLOC(char, fsize + 1);
	fseek(fp, 0, SEEK_SET);
	fread(str_pool, fsize, 1, fp);
	fclose(fp);

	vector_init(lines);

	char *p = str_pool;
	while (p < str_pool + fsize) {
		char *line_head = p;

		int have_colon_in = 0;
		for (int hex = 0; *p != '\n'; p++) {
			hex += ('0' <= p[0] && p[0] <= '9');
			hex += ('a' <= p[0] && p[0] <= 'f');
			hex += ('A' <= p[0] && p[0] <= 'F');
			have_colon_in += hex && p[1] == ':' && p[2] != '\n';
		}

		if (have_colon_in) {
			vector_push(lines, line_head);
		}

		p[0] = 0;
		p++;
	}

	vector_fixup(lines);
	bitmap_init(vector_size(lines), &visited);

	return 0;
}

void decoder_unload()
{
	vector_fini(lines);
	free(str_pool);
	bitmap_fini(&visited);
}

int get_addr_by_line(uint32_t line)
{
	char buf[4];
	int addr;

	int ret = sscanf(lines[line], "%x%[:]", &addr, buf);
	if (ret != 2) {
		printf("FATAL ERROR: sscanf (%s) return != 2\n", lines[line]);
		exit(0);
	}
	return addr;
}

int get_line_by_addr(uint32_t addr)
{
	int l = 0, r = vector_size(lines) - 1;

	int ret;
	while (l <= r) {
		int mid = (l + r)/2;
		int line = get_addr_by_line(mid);

		if (line == addr) {
			return mid;
		}

		if (line > addr) {
			r = mid - 1;
		} else {
			l = mid + 1;
		}
	}
	error(-1, "%s failed", __func__);
}

struct instruction get_instruction_by_line(uint32_t line)
{
	struct instruction i;
	char instr[128];
	sscanf(lines[line], "%x:%x\t %[^/]",
			&i.addr, &i.code, instr);

	char *p = instr + strlen(instr) - 1;
	while (*p <= ' ') {
		p--;
	}
	p[1] = 0;

	strncpy(i.string, instr, sizeof(i.string));

	return i;
}

int _get_ib_by_addr(uint32_t addr, bool until_jump, struct uint32_list *aq, struct instruction_block *rv)
{
	if (addr % 4) {
		error(-EINVAL, "%s addr align failed", __func__);
	}

	vector_define(struct instruction, instrs);
	vector_init(instrs);

	int line = get_line_by_addr(addr);
	if (line < 0) {
		error(-ENOADDR, "%s no such addr", __func__);
	}

	while (!bitmap_get(&visited, line)) {
		if (line >= vector_size(lines)) {
			break;
		}
		struct instruction i = get_instruction_by_line(line);
		vector_push(instrs, i);
		bitmap_set(&visited, line++);

		int is_jump_instr = strstr(i.string, "ret") == i.string;
		is_jump_instr += strstr(i.string, "eret") == i.string;
		is_jump_instr += strstr(i.string, "br") == i.string;
		if (is_jump_instr) {
			break;
		}

		uint32_t new_addr;
		int ret = get_jump_addr_by_str(i.string, &new_addr);
		if (ret < 0) {
			return ret;
		}
		if (ret == 0) {
			uint32_list_push(aq, new_addr);

			if (strstr(i.string, "b\t") == i.string) {
				break;
			}

			if (until_jump) {
				uint32_list_push(aq, i.addr + 4);
				break;
			} else {
				continue;
			}
		}
	}

	vector_fixup(instrs);

	if (vector_size(instrs)) {
		rv->ip = instrs;
		rv->start_addr = addr;
		rv->end_addr = addr + vector_size(instrs) * 4 - 4;
		return 0;
	} else {
		return 1;
	}

	return 0;
}

int get_inst_block_by_addr_until_jump(uint32_t start_addr,
		struct uint32_list *addr_queue,
		struct instruction_block *retval)
{
	return _get_ib_by_addr(start_addr, true, addr_queue, retval);
}

/* get_instruction_block_by_addr
 * 返回从start_addr开始的连续指令块
 * 并且把遇到的跳转指令存到addr_queue中 */
/* retval < 0: error code
 * retval = 0: success
 * retval > 0: no error, but failed */
int get_instruction_block_by_addr(uint32_t start_addr,
		struct uint32_list *addr_queue,
		struct instruction_block *retval)
{
	return _get_ib_by_addr(start_addr, false, addr_queue, retval);
}

void clear_visited()
{
	bitmap_clear(&visited);
}

void save_visited(struct bitmap *bm)
{
	bitmap_init(vector_size(lines), bm);
	bm->len = visited.len;
	memcpy(bm->ptr, visited.ptr, visited.len * sizeof(*visited.ptr));
}

void load_visited(struct bitmap *bm)
{
	visited.len = bm->len;
	memcpy(visited.ptr, bm->ptr, visited.len * sizeof(*visited.ptr));
	bitmap_fini(bm);
}

int get_func_by_addr(uint32_t start_addr, int clear,
		struct list_head *instruction_block_list)
{
	if (start_addr % 4) {
		error(-EINVAL, "%s addr align failed", __func__);
	}

	int ret = 0;
	uint32_list_define(queue);
	LIST_HEAD(tmp_i10s_list);
	uint32_list_push(&queue, start_addr);
	if (clear) {
		bitmap_clear(&visited);
	}

	while (!uint32_list_empty(&queue)) {
		struct instruction_block *i10s = MALLOC(struct instruction_block, 1);
		int addr = uint32_list_pop(&queue);

		ret = get_instruction_block_by_addr(addr, &queue, i10s);
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
		uint32_t min_addr = 0xffffffff;

		list_for_each_entry_safe(item, tmp, &tmp_i10s_list, list) {
			if (item->start_addr < min_addr) {
				min_addr = item->start_addr;
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
