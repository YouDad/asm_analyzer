#pragma once
#include "common.h"
#include "utils/rbtree.h"

/* map是映射表，用红黑树实现
 * _________________________
 * map_define(key_type, val_type, id)
 *     定义一个键类型是key_type，值类型是val_type的
 *     变量名叫id的map。
 *     如果key_type不支持'<'和'>'的话，需要使用
 *     map_define_with_cmp来支持比较
 *     -1代表'<', 0代表'=', 1代表'>'
 *
 * map_init(id) 初始化map
 * map_get(id, k, vp) 获得map[k]
 *     返回值代表0没有k，1有k
 *     *vp = retval
 * map_set(id, k, va) map[k] = v
 * map_fini(id) 回收map的资源
 * */

#define map_define(key_type, val_type, id) \
	map_define_with_cmp(key_type, val_type, id, NULL)

#define map_define_with_cmp(key_type, val_type, id, cmp) \
	struct $##id##_type {                                \
		struct rb_node node;                             \
		key_type key;                                    \
		val_type val;                                    \
	};                                                   \
	int (* $##id##_cmp)(key_type a, key_type b) = cmp;   \
	struct rb_root id

#define map_init(id) do { id = RB_ROOT; } while (0)

#define map_get(id, k, vp)                                \
({                                                        \
	struct rb_node *cur = id.rb_node;                     \
	int have = 0;                                         \
	while (cur) {                                         \
		struct $##id##_type *cur_data =                   \
			container_of(cur, struct $##id##_type, node); \
                                                          \
		if (!$##id##_cmp) {                               \
			if (k < cur_data->key) {                      \
				cur = cur->rb_left;                       \
			} else if (k > cur_data->key) {               \
				cur = cur->rb_right;                      \
			} else {                                      \
				*vp = cur_data->val;                      \
				have = 1;                                 \
				break;                                    \
			}                                             \
			continue;                                     \
		}                                                 \
		if ($##id##_cmp(k, cur_data->key) < 0) {          \
			cur = cur->rb_left;                           \
		} else if ($##id##_cmp(k, cur_data->key) > 0) {   \
			cur = cur->rb_right;                          \
		} else {                                          \
			*vp = cur_data->val;                          \
			have = 1;                                     \
			break;                                        \
		}                                                 \
	}                                                     \
	have;                                                 \
})

#define map_set(id, k, v)                                  \
do {                                                       \
	struct rb_node **ptr = &id.rb_node;                    \
	struct rb_node *parent = NULL;                         \
	int found = 0;                                         \
                                                           \
	for (struct rb_node *node = *ptr; node; node = *ptr) { \
		parent = node;                                     \
                                                           \
		struct $##id##_type *i =                           \
			container_of(node, struct $##id##_type, node); \
                                                           \
		if (!$##id##_cmp) {                                \
			if (k < i->key) {                              \
				ptr = &node->rb_left;                      \
			} else if (k > i->key) {                       \
				ptr = &node->rb_right;                     \
			} else {                                       \
				i->val = v;                                \
				found = 1;                                 \
				break;                                     \
			}                                              \
			continue;                                      \
		}                                                  \
		if ($##id##_cmp(k, i->key) < 0) {                  \
			ptr = &node->rb_left;                          \
		} else if ($##id##_cmp(k, i->key) > 0) {           \
			ptr = &node->rb_right;                         \
		} else {                                           \
			i->val = v;                                    \
			found = 1;                                     \
			break;                                         \
		}                                                  \
	}                                                      \
                                                           \
	if (!found) {                                          \
		struct $##id##_type *data =                        \
			MALLOC(struct $##id##_type, 1);                \
		data->key = k;                                     \
		data->val = v;                                     \
                                                           \
		rb_link_node(&data->node, parent, ptr);            \
		rb_insert_color(&data->node, &id);                 \
	}                                                      \
} while (0)

#define map_first(id) \
	rb_entry(rb_first(&id), struct $##id##_type, node)

#define map_next(id, node) \
	rb_entry(rb_next(node), struct $##id##_type, node)

#define map_fini(id)                                \
do {                                                \
    struct $##id##_type *n = NULL;                  \
    for (n = map_first(id); n; n = map_first(id)) { \
        if (n) {                                    \
            rb_erase(&n->node, &id);                \
            free(n);                                \
        }                                           \
    }                                               \
} while (0)
