#pragma once
#include <stddef.h>
#include <stdlib.h>

// ----------------------------------
// linked list node
// 
// ----------------------------------
struct list_node_s {
	struct list_node_s *p_last;
	struct list_node_s *p_next;
};

// ----------------------------------
// linked list instance
// 
// ----------------------------------
struct list_t {
	struct list_node_s *p_head;
	struct list_node_s *p_tail;
};

#define list_alloc_func(s) calloc(s, 1)
#define list_free_func(p) free(p)

#define list_alloc_node(_size) ((list_node_s *)list_alloc_func(sizeof(struct list_node_s) + _size))
#define list_delete_node(p) list_free_func((void *)p)

#define list_node_data(nodeptr, type) ((type *)(((char *)nodeptr) + sizeof(struct list_node_s)))

void         list_init(list_t *p_list);
void         list_free(list_t *p_list);

void         list_node_insert(struct list_node_s *p_last_node, struct list_node_s *p_node);
void         list_node_remove(struct list_node_s *p_node);
list_node_s *list_append_node(list_t *p_list, size_t _elem_size);
void         list_remove_node(list_t *p_list, list_node_s *p_node);
