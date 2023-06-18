#include "gl_shader.h"

gl_shader_source::gl_shader_source(const char *p_filename)
{
}

gl_shader_source::gl_shader_source(const char *p_source, size_t length)
{
}

gl_shader_source::~gl_shader_source()
{
}

const char *gl_shader_source::get_source()
{
	return p_soruce;
}

const size_t gl_shader_source::get_source_length()
{
	return size_t();
}

gl_shader_object::gl_shader_object()
{
}

gl_shader_object::~gl_shader_object()
{
}
