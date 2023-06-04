#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define HOBJECT_MAGIC ('JBOH')

typedef struct handle_object_s {
	uint32_t magic;
	uint32_t flags;
	size_t size;
	struct handle_object_s *p_last;
	struct handle_object_s *p_next;
	uint8_t data[];
} handle_object_t;

typedef enum HANDLE_OBJECT_STATUS_ {
	HANDLE_OBJECT_STATUS_OK = 0,
	HANDLE_OBJECT_ALLOCATION_FAILED,
	HANDLE_OBJECT_INVALID,
	HANDLE_OBJECT_GROUP_INVALID
} HANDLE_OBJECT_STATUS;

typedef enum HANDLE_GROUP_ {
	HANDLE_GROUP_GENERAL = 0,
	HANDLE_GROUP_RESOURCES,
	HANDLE_GROUP_ENTITIES,
	HANDLE_GROUP_PLUGINS,

	HANDLE_GROUP_MAX_GROUPS
} HANDLE_GROUP;

typedef struct handle_s { void *ptr; } handle_t;

#define HANDLE_OBJECT_DATA(p_handle)              ((uint8_t *)p_handle + sizeof(handle_object_t))
#define HANDLE_OBJECT_DATA_SECTION_SIZE(p_handle) (((handle_object_t *)p_handle)->size)

typedef enum HANDLE_NOTIFY_ACTION_ {
	HANDLE_NOTIFY_CREATE = 0,
	HANDLE_NOTIFY_DEATH
} HANDLE_NOTIFY_ACTION;

typedef bool(*handle_proc_t)(HANDLE_NOTIFY_ACTION action, handle_t h_created_object,
	handle_t h_parent_object,
	HANDLE_GROUP object_group);

typedef struct handle_group_s {
	struct handle_object_s *p_head;
	struct handle_object_s *p_tail;
} handle_group_t;

typedef struct hsys_context_s {
	size_t total_allocations;
	size_t total_alive_handles;
	void *(*alloc_proc)(size_t _size);
	void *(*free_proc)(void *ptr);
	handle_group_t groups[HANDLE_GROUP_MAX_GROUPS];
	handle_proc_t p_handle_creation_notify_proc;
} hsys_context_t;

/*
 Handle object info structure
*/
typedef struct hobject_info_s {
	uint32_t flags;
	size_t size;
} hobject_info_t;

/*
 Handles system initializing
*/
void                hsys_initialize(hsys_context_t *p_hsysctx, void *(*p_alloc_proc)(size_t _size), void *(*p_free_proc)(void *ptr));
void                hsys_deinitialize(hsys_context_t *p_hsysctx);

/*
 Internal handles API implementations
*/
HANDLE_OBJECT_STATUS hsys_hobject_create(hsys_context_t *p_hsysctx, handle_object_t **p_dst_new, HANDLE_GROUP group, const char *p_name, uint32_t flags, void *p_data, size_t object_size);
HANDLE_OBJECT_STATUS hsys_hobject_clone(hsys_context_t *p_hsysctx, handle_object_t **p_dst_clone, handle_object_t *p_src);
HANDLE_OBJECT_STATUS hsys_hobject_kill(hsys_context_t *p_hsysctx, handle_object_t *p_src);
bool                 hsys_hobject_alive(const handle_object_t *p_src);
uint8_t             *hsys_hobject_data_ptr(const handle_object_t *p_src);
bool                 hsys_hobject_get_info(hobject_info_t *p_dstinfo, const handle_object_t *p_src);
void                 hsys_register_object_creation_notify(handle_proc_t p_notifyproc);