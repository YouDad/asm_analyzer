#pragma once
#include "list.h"
#include <stdint.h>
#include <malloc.h>

struct uint32_list_item {
	struct list_head list;
	uint32_t item;
};

static inline void uint32_list_insert_head(struct list_head *head, uint32_t x)
{
	struct uint32_list_item *add_address;
	add_address = (struct uint32_list_item *)
		malloc(sizeof(struct uint32_list_item));
	INIT_LIST_HEAD(&add_address->list);
	add_address->item = x;
	list_add(&add_address->list, head);
}

static inline void uint32_list_insert_tail(struct list_head *head, uint32_t x)
{
	uint32_list_insert_head(head->prev, x);
}

static inline uint32_t uint32_list_pop_head(struct list_head *head)
{
	struct uint32_list_item *item =
		list_entry(head->next, struct uint32_list_item, list);

	uint32_t ret = item->item;
	list_del(head->next);
	free(item);

	return ret;
}

static inline uint32_t uint32_list_pop_tail(struct list_head *head)
{
	struct uint32_list_item *item =
		list_entry(head->prev, struct uint32_list_item, list);

	uint32_t ret = item->item;
	list_del(head->prev);
	free(item);

	return ret;
}
