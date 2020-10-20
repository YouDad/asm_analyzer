#include "types.h"
#include "common.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <malloc.h>

void print_instruction(struct instruction *i, char *pre)
{
	printf("%sinstruction{ addr: %04x, code: %x, string: \"%s\" }\n",
			pre, i->addr, i->code, i->string);
}

void print_instruction_short(struct instruction *i, char *pre)
{
	printf("%s%04x\t\t%s\n", pre, i->addr, i->string);
}

void print_instruction_block(struct instruction_block *ib)
{
	int ib_size = 4;
	ib_size += ib->end_addr - ib->start_addr;
	ib_size /= 4;

	printf("instruction_block[%d] {\n  start_addr: %x, end_addr: %x\n",
			ib_size, ib->start_addr, ib->end_addr);

	for (int i = 0; i < ib_size; i++) {
		print_instruction(&ib->ip[i], "  ");
	}

	printf("}\n");
}

void print_instruction_block_short(struct instruction_block *ib)
{
	int ib_size = 4;
	ib_size += ib->end_addr - ib->start_addr;
	ib_size /= 4;

	printf("instruction_block[%d] {\n  start_addr: %x, end_addr: %x\n",
			ib_size, ib->start_addr, ib->end_addr);

	for (int i = 0; i < ib_size; i++) {
		print_instruction_short(&ib->ip[i], "  ");
	}

	printf("}\n");
}

void release_instruction_block(struct instruction_block *ib)
{
	free(ib->ip);
	free(ib);
}

int can_merge_instruction_block(struct instruction_block *prev, struct instruction_block *next)
{
	return prev->end_addr + 4 == next->start_addr;
}

void merge_instruction_block(struct instruction_block *prev, struct instruction_block *next)
{
	int prev_size = (prev->end_addr - prev->start_addr + 4) / 4;
	int next_size = (next->end_addr - next->start_addr + 4) / 4;
	int size = prev_size + next_size;

	prev->ip = (struct instruction *)realloc(prev->ip, size * sizeof(struct instruction));
	memcpy(&prev->ip[prev_size], next->ip, next_size * sizeof(struct instruction));
	prev->end_addr = next->end_addr;
}

static inline int is_digit(char x)
{
	return '0' <= x && x <= '9';
}

static inline int is_word(char x)
{
	return is_digit(x) ||
		('a' <= x && x <= 'z') ||
		('A' <= x && x <= 'Z') || x == '_';
}

void get_regs_in_instruction(const char *str, int nskip,
		int nwrite, struct uint32_list *r, struct uint32_list *w)
{
	int regs[8];
	int cnt = 0;

	const char *p = str;
	while (*p && *p != '\t') p++;
	if (*p == '\t') p++;

	while (*p) {
		if (p[0] == 'x' || p[0] == 'w') {
			if (is_digit(p[1]) && !is_word(p[-1])) {
				if (is_digit(p[2])) {
					regs[cnt++] = (p[1] - '0') * 10 + p[2] - '0';
				} else {
					regs[cnt++] = p[1] - '0';
				}
			}
		}
		if (p[0] == 's') {
			if (p[1] == 'p' && !is_word(p[2]) && !is_word(p[-1])) {
				regs[cnt++] = 31;
			}
		}

		p++;
	}

	if (w) {
		for (int i = nskip; i < nskip + nwrite; i++) {
			uint32_list_push(w, regs[i]);
		}
	}

	if (r) {
		for (int i = 0; i < nskip; i++) {
			uint32_list_push(r, regs[i]);
		}
		for (int i = nskip + nwrite; i < cnt; i++) {
			uint32_list_push(r, regs[i]);
		}
	}
}

void get_index_reg_in_instruction(const char *str, struct uint32_list *w)
{
	const char *p = str, *q;
	int reg;
	while (*p && *p != '\t') p++;
	if (*p == '\t') p++;

	while (*p) {
		if (p[0] == '[') {
			q = p;
		}
		p++;
	}

	p = q+1;
	if (p[0] == 'x' || p[0] == 'w') {
		if (is_digit(p[2])) {
			reg = (p[1] - '0') * 10 + p[2] - '0';
		} else {
			reg = p[1] - '0';
		}
	} else if (p[0] == 's') {
		if (p[1] == 'p' && !is_word(p[2])) {
			reg = 31;
		}
	} else {
		printf("????%s\n", q);
		exit(0);
	}

	if (w) {
		uint32_list_push(w, reg);
	}
}

int is_pre_index(const char *str)
{
	return strstr(str, "!") != 0;
}

int is_post_index(const char *str)
{
	return !is_pre_index(str) && str[strlen(str) - 1] != ']';
}

struct inst_regs_relation {
	int nwrite;
	int nskip;
	const char *prefix;
};

struct inst_regs_relation base_inst_set[] = {
	{1, 0, "ad"}, // c, cs, d, dg, ds, r, rp
	{1, 0, "and"}, // and, ands
	{1, 0, "asr"}, // asr, asrv
	{0, 0, "at\t"},
	{1, 0, "aut"}, // da, dza, db, dzb, ia, ia1716, iasp, iaz, iza, ib, ib1716, ibsp, isz, izb
	{0, 0, "axflag\0"},
	{0, 0, "b."}, // b.cond
	{0, 0, "b\t"},
	{1, 0, "bf"}, // bfc, bfi, bfm, bfxi
	{1, 0, "bic"}, // bic, bics
	{0, 0, "bl\t"},
	{0, 0, "blr"}, // blr, aa, aaz, ab, abz
	{0, 0, "br"}, // br, aa, aaz, ab, abz, brk
	{0, 0, "bti\t"},
	{2, 0, "casp"}, // casp, caspa, caspal, caspl
	{1, 0, "cas"}, // b, ab, alb, lb, h, ah, alh, lh, cas, a, al, l
	{0, 0, "cb"}, // cbz, cbnz
	{0, 0, "ccm"}, // ccmn, ccmp
	{0, 0, "cf"}, // cfinv, cfp
	{1, 0, "cin"}, // cinc, cinv
	{0, 0, "clrex\t"},
	{1, 0, "cl"}, // cls, clz
	{0, 0, "cm"}, // cmn, cmp, cmpp
	{1, 0, "cneg\t"},
	{0, 0, "cpp\t"},
	{1, 0, "crc32"}, // b, h, w, x, cb, ch, cw, cx
	{0, 0, "csdb\0"},
	{1, 0, "cs"}, // csel, cset, csetm, csinc, csinv, csneg
	{0, 0, "dc"}, // dc, dcps1, dcps2, dcps3
	{0, 0, "dgh\0"},
	{0, 0, "dmb\t"},
	{0, 0, "drps\0"},
	{0, 0, "dsb\t"},
	{0, 0, "dvp\t"},
	{1, 0, "eon\t"},
	{1, 0, "eor\t"},
	{0, 0, "eret"}, // eret, ereta, eretb
	{0, 0, "esb\0"},
	{1, 0, "extr\t"},
	{1, 0, "gmi\t"},
	{0, 0, "hint\t"},
	{0, 0, "hlt\t"},
	{0, 0, "hvc\t"},
	{0, 0, "ic\t"},
	{1, 0, "irg\t"},
	{0, 0, "isb\0"},
	{2, 0, "ldaxp\t"},
	{2, 0, "ldnp\t"},
	{2, 0, "ldp\t"},
	{2, 0, "ldpsw\t"},
	{2, 0, "ldxp\t"},
	{1, 1, "ldadd"}, // b, lb, h, lh, ldadd, l
	{1, 1, "ldclr"}, // b, lb, h, lh, ldclr, l
	{1, 1, "ldeor"}, // b, lb, h, lh, ldeor, l
	{1, 1, "ldset"}, // b, lb, h, lh, ldset, l
	{1, 1, "ldsmax"}, // b, lb, h, lh, ldsmax, l
	{1, 1, "ldsmin"}, // b, lb, h, lh, ldsmin, l
	{1, 1, "ldumax"}, // b, lb, h, lh, ldumax, l
	{1, 1, "ldumin"}, // b, lb, h, lh, ldumin, l
	{1, 0, "ld"}, // ...
	{1, 0, "madd\t"},
	{1, 0, "mneg\t"},
	{1, 0, "mov"}, // mov, movk, movz, movn
	{0, 0, "mrs\t"},
	{0, 0, "msr\t"},
	{1, 0, "msub\t"},
	{1, 0, "mul\t"},
	{1, 0, "mvn\t"},
	{1, 0, "neg"}, // neg, negs
	{1, 0, "ngc"}, // ngc, ngcs
	{0, 0, "nop\0"},
	{1, 0, "or"}, // orn, orr
	{1, 0, "pacd"}, // a, za, b, zb
	{1, 0, "pacga\t"},
	{1, 0, "paci"}, // a, a1716, asp, az, za, b, b1716, bsp, bz, zb
	{0, 0, "prf"}, // prfm, prfum
	{0, 0, "psb"}, // ?
	{0, 0, "pssbb\0"},
	{1, 0, "rbit\t"},
	{0, 0, "ret"}, // ret, retaa, retab
	{1, 0, "rev"}, // rev, 16, 32, 64
	{0, 0, "rmif\t"},
	{1, 0, "ror"}, // ror, rorv
	{1, 0, "sbc"}, // sbc, sbcs
	{1, 0, "sbf"}, // sbfiz, sbfm, sbfx
	{0, 0, "sb\0"},
	{1, 0, "sdiv\t"},
	{0, 0, "setf"}, // setf8, setf16
	{0, 0, "sev"}, // sev, sevl
	{0, 0, "smc\t"},
	{1, 0, "sm"}, // smaddl, smnegl, smsubl, smulh, smull
	{0, 0, "ssbb\0"},
	{1, 1, "stadd"}, // b, lb, h, lh, stadd, l
	{1, 1, "stclr"}, // b, lb, h, lh, stclr, l
	{1, 1, "steor"}, // b, lb, h, lh, steor, l
	{1, 0, "stlx"}, // p, r, rb, rh
	{1, 1, "stset"}, // b, lb, h, lh, stset, l
	{1, 1, "stsmax"}, // b, lb, h, lh, stsmax, l
	{1, 1, "stsmin"}, // b, lb, h, lh, stsmin, l
	{1, 1, "stumax"}, // b, lb, h, lh, stumax, l
	{1, 1, "stumin"}, // b, lb, h, lh, stumin, l
	{1, 0, "stx"}, // p, r, rb, rh
	{0, 0, "st"}, // ...
	{1, 0, "sub"}, // sub, g, p, ps, s
	{0, 0, "svc\t"},
	{1, 1, "swp"}, // b, ab, alb, lb, h, ah, alh, lh, swp, a, al, l
	{1, 0, "sxt"}, // b, h, w
	{0, 0, "sys"}, // sys, l
	{0, 0, "tbnz\t"},
	{0, 0, "tbz\t"},
	{0, 0, "tlbi\t"},
	{0, 0, "tsb"}, // ?
	{0, 0, "tst\t"},
	{1, 0, "ubf"}, // iz, m, x
	{0, 0, "udf\t"},
	{1, 0, "udiv\t"},
	{1, 0, "um"}, // addl, negl, subl, ulh, ull
	{1, 0, "uxt"}, // uxtb, uxth
	{0, 0, "wf"}, // wfi, wfe
	{0, 0, "xaflag\0"},
	{1, 0, "xpac"}, // d, i, lri
	{0, 0, "yield\0"},
};

const char *pre_index[] = {
	"ldp\t",
	"ldpsw\t",
	"ldr\t",
	"ldraa\t",
	"ldrab\t",
	"ldrb\t",
	"ldrh\t",
	"ldrsb\t",
	"ldrsh\t",
	"ldrsw\t",
	"st2g\t",
	"stg\t",
	"stgp\t",
	"stp\t",
	"str\t",
	"strb\t",
	"strh\t",
	"stz2g\t",
	"stzg\t",
};

const char *post_index[] = {
	"ldp\t",
	"ldpsw\t",
	"ldr\t",
	"ldrb\t",
	"ldrh\t",
	"ldrsb\t",
	"ldrsh\t",
	"ldrsw\t",
	"st2g\t",
	"stg\t",
	"stgp\t",
	"stp\t",
	"str\t",
	"strb\t",
	"strh\t",
	"stz2g\t",
	"stzg\t",
};

int regs_affected_by_inst(struct instruction *i, struct uint32_list *r, struct uint32_list *w)
{
	if (strstr(i->string, "bl") == i->string) {
		uint32_list_push(w, 30);
	}

	int processed = 0;
	for (int j = 0; j < array_size(base_inst_set); j++) {
		struct inst_regs_relation *inst = &base_inst_set[j];
		if (strstr(i->string, inst->prefix) == i->string) {
			get_regs_in_instruction(i->string, inst->nskip, inst->nwrite, r, w);
			processed = 1;
			break;
		}
	}

	int indexed = 0;
	for (int j = 0; j < array_size(pre_index); j++) {
		if (strstr(i->string, pre_index[j]) == i->string) {
			if (is_pre_index(i->string)) {
				get_index_reg_in_instruction(i->string, w);
				indexed = 1;
			}
			break;
		}
	}

	if (!indexed) {
		for (int j = 0; j < array_size(post_index); j++) {
			if (strstr(i->string, post_index[j]) == i->string) {
				if (is_post_index(i->string)) {
					get_index_reg_in_instruction(i->string, w);
				}
				break;
			}
		}
	}

	return !processed;
}
