#include "buffer.h"

int render_alloc_buffer(r_buffer_s *p_buffer, BUFFER_TYPE type, BUFFER_ACCESS_FREQUENCY accessfreq, size_t size, void *p_data)
{
	p_buffer->type = type;
	p_buffer->afreq = accessfreq;
	p_buffer->size = size;
	glGenBuffers(1, &p_buffer->buffer);

	return 0;
}

int render_realloc_buffer(r_buffer_s *p_buffer, size_t newsize, bool bnocopy)
{
	return 0;
}

int render_copy_buffer(r_buffer_s *p_dst_buffer, r_buffer_s *p_src_buffer)
{
	return 0;
}

int render_buffer_get_size(r_buffer_s *p_buffer)
{
	return 0;
}

int render_buffer_get_data(void **p_dst_ptr, r_buffer_s *p_buffer)
{
	return 0;
}

int render_buffer_set_data(r_buffer_s *p_buffer, void *p_data, size_t from_idx, size_t count)
{
	return 0;
}

int render_map_buffer(void **p_dst_ptr, r_buffer_s *p_buffer, BUFFER_ACCESS access)
{
	return 0;
}

int render_unmap_buffer(r_buffer_s *p_buffer)
{
	return 0;
}

int render_free_buffer(r_buffer_s *p_buffer)
{
	return 0;
}
