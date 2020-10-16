#pragma once

struct list_head {
	struct list_head *next, *prev;
};

static inline void INIT_LIST_HEAD(struct list_head *list)
{
	list->next = list;
	list->prev = list;
}

#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define LIST_HEAD(name) \
	struct list_head name = LIST_HEAD_INIT(name)

#define container_of(ptr, type, member) ({                 \
	const __typeof__(((type *)0)->member) *__mptr = (ptr); \
	(type *)((char *)__mptr - offsetof(type, member));      \
})

static inline void __list_add(struct list_head *_new, 
		struct list_head *prev,
		struct list_head *next)
{
	next->prev = _new;
	_new->next = next;
	_new->prev = prev;
	prev->next = _new;
}

static inline void list_insert_head(struct list_head *_new, struct list_head *head)
{
	__list_add(_new, head, head->next);
}

static inline void list_insert_tail(struct list_head *_new, struct list_head *head)
{
	__list_add(_new, head->prev, head);
}

static inline void __list_del(struct list_head *prev, struct list_head *next)
{
	next->prev = prev;
	prev->next = next;
}

static inline void list_del(struct list_head *entry)
{
	__list_del(entry->prev, entry->next);
	entry->next = 0;
	entry->prev = 0;
}

static inline int list_empty(const struct list_head *head)
{
	return head->next == head;
}

#define list_entry(ptr, type, member) container_of(ptr, type, member)

#define list_first_entry(ptr, type, member) \
	list_entry((ptr)->next, type, member)

#define list_next_entry(pos, member) \
	list_entry((pos)->member.next, typeof(*(pos)), member)

#define list_for_each_entry(pos, head, member)           \
for (pos = list_first_entry(head, typeof(*pos), member); \
	&pos->member != (head);                              \
	pos = list_next_entry(pos, member))

#define list_for_each_entry_safe(pos, tmp, head, member) \
for (pos = list_first_entry(head, typeof(*pos), member), \
	tmp = list_next_entry(pos, member);                  \
	&pos->member != (head);                              \
	pos = tmp, tmp = list_next_entry(tmp, member))
