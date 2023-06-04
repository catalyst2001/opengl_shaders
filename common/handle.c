#include "handle.h"

void hsys_initialize(hsys_context_t *p_hsysctx, void *(*p_alloc_proc)(size_t _size), void *(*p_free_proc)(void *ptr))
{
	for (size_t i = 0; i < sizeof(p_hsysctx->groups) / sizeof(p_hsysctx->groups[0]); i++)
		p_hsysctx->groups[i].p_head = p_hsysctx->groups[i].p_tail = NULL;

	p_hsysctx->total_alive_handles = 0;
	p_hsysctx->total_allocations = 0;
	p_hsysctx->alloc_proc = p_alloc_proc;
	p_hsysctx->free_proc = p_free_proc;
	p_hsysctx->p_handle_creation_notify_proc = NULL;
}

void hsys_deinitialize(hsys_context_t *p_hsysctx)
{
	/* clear each handles group */
	for (size_t i = 0; i < sizeof(p_hsysctx->groups) / sizeof(p_hsysctx->groups[0]); i++) {
		handle_object_t *p_curr_object, *p_last_object;
		if (p_hsysctx->groups[i].p_head) {
			p_curr_object = p_hsysctx->groups[i].p_head;
			while (p_curr_object) {
				p_last_object = p_curr_object;
				p_curr_object = p_curr_object->p_next;
				p_hsysctx->free_proc(p_last_object);
			}
		}
	}
	p_hsysctx->total_alive_handles = 0;
	p_hsysctx->total_allocations = 0;
}

HANDLE_OBJECT_STATUS hsys_hobject_create(hsys_context_t *p_hsysctx, handle_object_t **p_dst_new, HANDLE_GROUP group, const char *p_name, uint32_t flags, void *p_data, size_t object_size)
{
	/* check group  */
	if (group < HANDLE_GROUP_GENERAL && group >= HANDLE_GROUP_MAX_GROUPS)
		return HANDLE_OBJECT_GROUP_INVALID;

	size_t object_block_size = sizeof(handle_object_t) + object_size;
	handle_object_t *p_object = (handle_object_t *)p_hsysctx->alloc_proc(object_block_size);
	if (!p_object)
		return HANDLE_OBJECT_ALLOCATION_FAILED;

	memset(p_object, 0, object_block_size);

	/* update context handles list */
	handle_group_t *p_handle_object_group = &p_hsysctx->groups[group];
	if (!p_handle_object_group->p_head) {
		p_handle_object_group->p_head = p_object;
	}

	if (!p_handle_object_group->p_tail) {
		p_handle_object_group->p_tail = p_object;
	}
	else
	{
		p_handle_object_group->p_tail->p_next = p_object;
		p_object->p_last = p_handle_object_group->p_tail;
		p_handle_object_group->p_tail = p_object;
	}
	*p_dst_new = p_object;
	p_hsysctx->total_alive_handles++;
	p_hsysctx->total_allocations++;
	return HANDLE_OBJECT_STATUS_OK;
}

HANDLE_OBJECT_STATUS hsys_hobject_clone(hsys_context_t *p_hsysctx, handle_object_t **p_dst_clone, handle_object_t *p_src)
{
	

	return HANDLE_OBJECT_STATUS_OK;
}

HANDLE_OBJECT_STATUS hsys_hobject_kill(hsys_context_t *p_hsysctx, handle_object_t *p_src)
{
	

	return HANDLE_OBJECT_STATUS_OK;
}

bool hsys_hobject_alive(const handle_object_t *p_src)
{
	return p_src && p_src->magic == HOBJECT_MAGIC;
}

uint8_t *hsys_hobject_data_ptr(const handle_object_t *p_src)
{
	if (!hsys_hobject_alive(p_src))
		return NULL;

	return &p_src->data[sizeof(handle_object_t)];
}

bool hsys_hobject_get_info(hobject_info_t *p_dstinfo, const handle_object_t *p_src)
{
	if (!hsys_hobject_alive(p_src))
		return false;

	p_dstinfo->flags = p_src->flags;
	p_dstinfo->size = p_src->size;
	return true;
}

void hsys_register_object_creation_notify(handle_proc_t p_notifyproc)
{
}
