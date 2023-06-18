#pragma once
#include "../common/glad/glad.h"

enum BUFFER_TYPE {
	BUFFER_TYPE_VERTEX_BUFFER_2D = 0,
	BUFFER_TYPE_VERTEX_BUFFER_3D,
	BUFFER_TYPE_INDEX_BUFFER
};

enum BUFFER_ACCESS {
	BUFFER_READONLY = 0,
	BUFFER_WRITEONLY
};

enum BUFFER_ACCESS_FREQUENCY {
	BUFFER_ACCESS_FREQUENCY_STATIC = 0,
	BUFFER_ACCESS_FREQUENCY_DYNAMIC
};

struct r_buffer_s {
	BUFFER_TYPE creation_type;
	BUFFER_ACCESS_FREQUENCY afreq;
	GLuint type;
	GLuint buffer;
	GLuint size;
	GLuint vertex_size;
	GLuint attribs_size;
};

int render_alloc_buffer(r_buffer_s *p_buffer, BUFFER_TYPE type, BUFFER_ACCESS_FREQUENCY accessfreq, size_t size, void *p_data);
int render_realloc_buffer(r_buffer_s *p_buffer, size_t newsize, bool bnocopy);
int render_copy_buffer(r_buffer_s *p_dst_buffer, r_buffer_s *p_src_buffer);
int render_buffer_get_size(r_buffer_s *p_buffer);
int render_buffer_get_data(void **p_dst_ptr, r_buffer_s *p_buffer);
int render_buffer_set_data(r_buffer_s *p_buffer, void *p_data, size_t from_idx, size_t count);
int render_map_buffer(void **p_dst_ptr, r_buffer_s *p_buffer, BUFFER_ACCESS access);
int render_unmap_buffer(r_buffer_s *p_buffer);
int render_free_buffer(r_buffer_s *p_buffer);