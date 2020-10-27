#pragma once
#include "common.h"

/* vector是一个动态容器，优点是不用担心大小，缺点是需要手动初始化以及释放
 * _________________________________________________________________________
 * vector_define(type, id) 定义一个vector，变量名叫id，可以直接id[0]访问元素
 *                         元素类型是type型的，类似于vector<type> id;
 *
 * vector_define_extern(type, id) 用于对外开放，参数和vector_define一样
 *
 * vector_struct_define(type, id) 在结构体中定义vector
 * vector_struct_init(id)         在结构体中初始化vector
 *
 * vector_init(vector)            初始化vector
 * vector_fixup(vector)           固定vector大小，节省内存
 * vector_fini(vector)            回收释放vector
 *
 * vector_size(vector)            返回vector已push元素个数
 * vector_push(vector, x)         向vector的尾部追加元素x
 * vector_pushs(vector, cnt, x)   向vector的尾部追加cnt个元素x
 *
 * 使用流程为: define, init, push..., [fixup]
 * 销毁流程为: fini
 * */

#define vector_define_extern(type, vector) \
	extern type *vector;                   \
	extern int vector##_cnt

#define vector_define(type, vector) \
	type *vector;                   \
	int vector##_cnt = 0

#define vector_init(vector)                            \
	int vector##_max = 4096 / sizeof(typeof(*vector)); \
	vector = MALLOC(typeof(*vector), vector##_max)

#define vector_struct_define(type, vector) \
	type *vector;                   \
	int vector##_cnt;               \
	int vector##_max

#define vector_struct_init(vector)                 \
	vector##_cnt = 0;                              \
	vector##_max = 4096 / sizeof(typeof(*vector)); \
	vector = MALLOC(typeof(*vector), vector##_max)

#define vector_fixup(vector)                                            \
	do {                                                                \
		if (vector##_cnt == 0) {                                        \
			vector = 0;                                                 \
			break;                                                      \
		}                                                               \
		void *new_ptr = REALLOC(vector, typeof(*vector), vector##_cnt); \
		if (!new_ptr) {                                                 \
			error(-ENOMEM, "realloc failed");                           \
		}                                                               \
		vector = new_ptr;                                               \
	} while (0)

#define vector_fini(vector) free(vector)
	
#define vector_size(vector) ({ vector##_cnt; })

#define vector_push(vector, val) ({                                     \
	if (vector##_cnt >= vector##_max) {                                 \
		vector##_max *= 2;                                              \
		void *new_ptr = REALLOC(vector, typeof(*vector), vector##_max); \
		if (!new_ptr) {                                                 \
			error(-ENOMEM, "realloc failed");                           \
		}                                                               \
		vector = new_ptr;                                               \
	}                                                                   \
	vector[vector##_cnt++] = val;                                       \
})

#define vector_expand(id, cnt, val) do {                        \
	int _cnt = ((cnt) + 1);                                     \
	int _val = (val);                                           \
	if (_cnt > vector_size(id)) {                               \
		if (_cnt > id##_max) {                                  \
			id##_max *= 2;                                      \
			void *new_ptr = REALLOC(id, typeof(*id), id##_max); \
			if (!new_ptr) {                                     \
				error(-ENOMEM, "realloc failed");               \
			}                                                   \
			id = new_ptr;                                       \
		}                                                       \
		while (id##_cnt <= _cnt) {                              \
			id[id##_cnt++] = _val;                              \
		}                                                       \
	}                                                           \
} while (0)
