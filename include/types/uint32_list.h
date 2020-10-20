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
	struct uint32_list_item *new_addr;
	new_addr = MALLOC(struct uint32_list_item, 1);
	INIT_LIST_HEAD(&new_addr->list);
	new_addr->item = x;
	list_insert_head(&new_addr->list, list->head.prev);
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

#define uint32_list_foreach(iter, uint32_list)                                      \
	for (struct uint32_list_item *iter =                                            \
			list_first_entry(&((uint32_list).head), struct uint32_list_item, list); \
			&iter->list != &((uint32_list).head);                                   \
			iter = list_next_entry(iter, list)                                      \
	)
