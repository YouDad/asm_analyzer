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
#include "armv8/ldr.h"
#include "armv8/mov.h"
#include "armv8/movk.h"
#include "armv8/mrs.h"
#include "armv8/msr.h"
#include "armv8/orr.h"
#include "armv8/ret.h"
#include "armv8/stp.h"
#include "armv8/str.h"
#include "armv8/sub.h"

typedef int (*translate_func_t)(const struct instruction *inst, char *str, int *str_cnt, int len);

struct asm_inst {
	const char *inst;
	translate_func_t translate;
	const char **test_inst;
	const char **test_result;
	int test_inst_len;
	int test_result_len;
};

#define array_size(x) (sizeof(x) / sizeof(typeof(x[0])))
#define DEFINE_INSTRUCTION(str, id) \
	{str, _translate_##id, _test_inst_##id, _test_result_##id, array_size(_test_inst_##id), array_size(_test_result_##id)}

static struct asm_inst armv8_insts[] = {
	DEFINE_INSTRUCTION("add\t",  add),
	DEFINE_INSTRUCTION("adrp\t", adrp),
	DEFINE_INSTRUCTION("adr\t",  adr),
	DEFINE_INSTRUCTION("b.",     b_cond),
	DEFINE_INSTRUCTION("bl\t",   bl),
	DEFINE_INSTRUCTION("b\t",    b),
	DEFINE_INSTRUCTION("cbnz\t", cbnz),
	DEFINE_INSTRUCTION("cbz\t",  cbz),
	DEFINE_INSTRUCTION("ldp\t",  ldp),
	DEFINE_INSTRUCTION("ldrb\t", ldrb),
	DEFINE_INSTRUCTION("ldr\t",  ldr),
	DEFINE_INSTRUCTION("movk\t", movk),
	DEFINE_INSTRUCTION("mov\t",  mov),
	DEFINE_INSTRUCTION("mrs\t",  mrs),
	DEFINE_INSTRUCTION("msr\t",  msr),
	DEFINE_INSTRUCTION("orr\t",  orr),
	DEFINE_INSTRUCTION("ret\0",  ret),
	DEFINE_INSTRUCTION("stp\t",  stp),
	DEFINE_INSTRUCTION("str\t",  str),
	DEFINE_INSTRUCTION("sub\t",  sub),
};
