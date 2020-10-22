#pragma once
#include "armv8/add.h"
#include "armv8/adr.h"
#include "armv8/adrp.h"
#include "armv8/b_cond.h"
#include "armv8/b.h"
#include "armv8/bl.h"
#include "armv8/cbnz.h"
#include "armv8/cbz.h"
#include "armv8/ldp.h"
#include "armv8/ldrb.h"
#include "armv8/mov.h"
#include "armv8/movk.h"
#include "armv8/mrs.h"
#include "armv8/msr.h"
#include "armv8/orr.h"
#include "armv8/stp.h"
#include "armv8/str.h"
#include "armv8/sub.h"

typedef int (*translate_func_t)(const struct instruction *inst, char *str, int *str_cnt, int len);

struct asm_inst {
	const char *inst;
	translate_func_t translate;
};

static struct asm_inst armv8_insts[] = {
	{"add\t", _translate_add, },
	{"adrp\t", _translate_adrp, },
	{"adr\t", _translate_adr, },
	{"b_cond\t", _translate_b_cond, },
	{"bl\t", _translate_bl, },
	{"b\t", _translate_b, },
	{"cbnz\t", _translate_cbnz, },
	{"cbz\t", _translate_cbz, },
	{"ldp\t", _translate_ldp, },
	{"ldrb\t", _translate_ldrb, },
	{"movk\t", _translate_movk, },
	{"mov\t", _translate_mov, },
	{"mrs\t", _translate_mrs, },
	{"msr\t", _translate_msr, },
	{"orr\t", _translate_orr, },
	{"stp\t", _translate_stp, },
	{"str\t", _translate_str, },
	{"sub\t", _translate_sub, },
};
