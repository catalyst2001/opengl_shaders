#include "list.h"

void list_node_insert(struct list_node_s *p_last_node, struct list_node_s *p_node)
{
	p_node->p_last = p_last_node;
	p_node->p_next = p_last_node->p_next;
	p_last_node->p_next = p_node;
}

void list_node_remove(struct list_node_s *p_node)
{
	struct list_node_s *p_last_node = p_node->p_last;
	struct list_node_s *p_next_node = p_node->p_next;
	if (p_last_node)
		p_last_node->p_next = p_last_node;

	if (p_next_node)
		p_next_node->p_last = p_last_node;
}

void list_init(list_t *p_list)
{
	p_list->p_head = NULL;
	p_list->p_tail = NULL;
}

void list_free(list_t *p_list)
{
	list_node_s *p_node = p_list->p_head;
	while (p_node) {
		list_delete_node(p_node);
		p_node = p_node->p_next;
	}
}

list_node_s *list_append_node(list_t *p_list, size_t _elem_size)
{
	list_node_s *p_node = list_alloc_node(_elem_size);
	if (!p_node)
		return NULL;

	if (!p_list->p_head)
		p_list->p_head = p_node;

	if (!p_list->p_tail) {
		list_node_insert(p_list->p_tail, p_node);
	}
	else {
		p_list->p_tail = p_node;
	}
	return p_node;
}

void list_remove_node(list_t *p_list, list_node_s *p_node)
{
	if (p_list->p_tail == p_node && p_list->p_tail)
		p_list->p_tail = p_list->p_tail->p_last;

	if (p_list->p_head == p_node && p_list->p_head)
		p_list->p_head = p_list->p_head->p_next;

	list_node_remove(p_node);
}