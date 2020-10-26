#include "utils/insts.h"
#include "utils/log.h"

static char cmp_r1[20];
static char cmp_r2[20];
static char cmp_addr;
static bool have_cmp = false;

void set_cmp(uint32_t addr, char *r1, char *r2)
{
	if (have_cmp) {
		warn("have_cmp! %x: %s, %s", cmp_addr, cmp_r1, cmp_r2);
	}
	have_cmp = true;
	cmp_addr = addr;
	strcpy(cmp_r1, r1);
	strcpy(cmp_r2, r2);
}

bool get_cmp(uint32_t *addr, char r1[], char r2[])
{
	if (have_cmp) {
		*addr = cmp_addr;
		strcpy(r1, cmp_r1);
		strcpy(r2, cmp_r2);
		have_cmp = false;
		return true;
	} else {
		/* warn("no cmp"); */
		return false;
	}
}
