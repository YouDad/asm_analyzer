#pragma once
#include "list.h"
#include "common.h"

struct uint32_list {
	struct list_head head;
};

struct uint32_list_item {
	struct list_head list;
	uint32_t item;
};

#define uint32_list_define(id) struct uint32_list id = {{ &id.head, &id.head }}

static inline void uint32_list_init(struct uint32_list *list)
{
	INIT_LIST_HEAD(&list->head);
}

static inline int uint32_list_empty(struct uint32_list *list)
{
	return list_empty(&list->head);
}

static inline void uint32_list_push(struct uint32_list *list, uint32_t x)
{
	struct uint32_list_item *add_address;
	add_address = (struct uint32_list_item *)
		malloc(sizeof(struct uint32_list_item));
	INIT_LIST_HEAD(&add_address->list);
	add_address->item = x;
	list_insert_head(&add_address->list, list->head.prev);
}

static inline uint32_t uint32_list_pop(struct uint32_list *list)
{
	struct uint32_list_item *item =
		list_entry(list->head.next, struct uint32_list_item, list);

	uint32_t ret = item->item;
	list_del(list->head.next);
	free(item);

	return ret;
}
