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
struct bitmap {
	bitmap_t *ptr;
	int len;
};

static inline int bitmap_init(unsigned int size, struct bitmap *bm)
{
	bm->len = (size + bes - 1) / bes * sizeof(bitmap_t);
	bm->ptr = (bitmap_t *)malloc(bm->len);
	if (!bm->ptr) {
		return -ENOMEM;
	}
	memset(bm->ptr, 0, bm->len);
	return 0;
}

static inline void bitmap_clear(struct bitmap *bm)
{
	memset(bm->ptr, 0, bm->len);
}

static inline void bitmap_set(struct bitmap *bm, int bit)
{
	bm->ptr[bit / bes] |= 1 << (bit % bes);
}

static inline int bitmap_get(struct bitmap *bm, int bit)
{
	return !!(bm->ptr[bit / bes] & (1 << (bit % bes)));
}

static inline void bitmap_fini(struct bitmap *bm)
{
	free(bm->ptr);
}
