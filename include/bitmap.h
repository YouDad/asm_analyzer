#pragma once
#include <string.h>
#include <errno.h>
#include <malloc.h>

/* bitmap位图
 * 生命周期为define(x)
 * -> init(&x, size)
 * -> is_set = get(x, bit) / set(x, bit)
 * -> fini(x) */

typedef unsigned int bitmap_t;
static const int bes = 8 * sizeof(bitmap_t);

#define bitmap_define(bitmap) bitmap_t *bitmap

static inline int bitmap_init(bitmap_t **bm, unsigned int size)
{
	int need_element_number = (size + bes - 1) / bes;
	*bm = (bitmap_t *)malloc(need_element_number * sizeof(bitmap_t));
	if (!bm) {
		return -ENOMEM;
	}
	memset(*bm, 0, need_element_number * sizeof(bitmap_t));
	return 0;
}

static inline void bitmap_set(bitmap_t *bm, int bit)
{
	bm[bit / bes] |= 1 << (bit % bes);
}

static inline int bitmap_get(bitmap_t *bm, int bit)
{
	return !!(bm[bit / bes] & (1 << (bit % bes)));
}

static inline void bitmap_fini(bitmap_t *bm)
{
	free(bm);
}
