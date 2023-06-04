#include <stdio.h>

#include "shader.h"

/* RENDERER SHADER TYPE TO GL SHADER TYPE CONST  */
GLuint gl_shader_type_to_gl_const(SHADER_TYPE shader_type)
{
	static GLuint shader_types[] = { GL_VERTEX_SHADER, GL_GEOMETRY_SHADER, GL_FRAGMENT_SHADER };
	if (shader_type < 0 || shader_type >= (sizeof(shader_types) / sizeof(shader_types[0])))
		return GL_NONE;

	return shader_types[shader_type];
}

bool gl_shader_source_compile(GLuint *p_dstshader, char *p_dsterr, size_t dstlen, GLenum shader_type, shader_info_t *p_shader_info)
{
	GLenum error;
	GLsizei log_length;
	GLint compile_status = GL_FALSE;
	GLuint shader = glCreateShader(shader_type);
	if (shader == GL_NONE) {
		error = glGetError();
		sprintf_s(p_dsterr, dstlen, "glCreateShader() error %d (0x%X)", error, error);
		return false;
	}

	/* set source and compile shader */
	glShaderSource(shader, 1, &p_shader_info->p_shader_source, NULL);
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);
	if (compile_status != GL_TRUE) {
		glGetShaderInfoLog(shader, (GLsizei)dstlen, &log_length, p_dsterr);
		glDeleteShader(shader);
		return false;
	}

	/* bind attribs locations and set atrribs data format */
	if (p_shader_info->p_attribs_bindings) {
		for (uint32_t i = 0; i < p_shader_info->num_attribs; i++) {
			shader_attrib_binding_t *p_attrib_binding = &p_shader_info->p_attribs_bindings[i];
			glBindAttribLocation(shader, p_attrib_binding->index, p_attrib_binding->p_attrib_name);
		}
	}
	*p_dstshader = shader;
	return true;
}

SHADER_PROG_STATUS gl_shader_prog_create_program(shader_program_t *p_dst, char *p_dsterr, size_t dstlen,
	uint32_t flags, const shader_info_t *p_shaders_infos, uint32_t shaders_count)
{
	uint32_t i;
	SHADER_PROG_STATUS status = SHADER_PROG_STATUS_OK;

	p_dst->flags = SHADER_FLAG_NONE;
	for (i = 0; i < SHADER_CNT(p_dst->shaders); i++)
		p_dst->shaders[i] = GL_NONE;

	/* compile shaders */
	for (i = 0; i < shaders_count; i++) {
		p_dst->flags |= (1 << i);
		GLenum shader_type = gl_shader_type_to_gl_const(p_shaders_infos[i].type);
		if (shader_type == GL_NONE) {
			status = SHADER_PROG_STATUS_INVALID_SHADER_TYPE;
			goto __error_cleanup;
		}

		if (!gl_shader_source_compile(&p_dst->shaders[i], p_dsterr, dstlen, shader_type, p_shaders_infos[i].p_shader_source)) {
			status = SHADER_PROG_STATUS_COMPILATION_ERROR;
			goto __error_cleanup;
		}
	}

	/* create shader program */
	p_dst->program = glCreateProgram();
	for (size_t i = 0; i < SHADER_CNT(p_dst->shaders); i++)
		if(p_dst->shaders[i] != GL_NONE)
			glAttachShader(p_dst->program, p_dst->vert_shader);

	glLinkProgram(p_dst->program);

	/* finish OK! */
	return SHADER_PROG_STATUS_OK;

	/* Cleanup if error occured */
__error_cleanup:

	/* delete created shaders */
	assert(0);

	return status;
}

SHADER_PROG_STATUS gl_shader_prog_delete_program(shader_program_t *p_src_prog)
{
	for (uint32_t i = 0; i < SHADER_CNT(p_src_prog->shaders); i++) {
		if (p_src_prog->shaders[i] != GL_NONE) {
			glDetachShader(p_src_prog->program, p_src_prog->shaders[i]);
			glDeleteShader(p_src_prog->shaders[i]);
			p_src_prog->shaders[i] = GL_NONE;
		}
	}
	glDeleteProgram(p_src_prog->program);
	return SHADER_PROG_STATUS_OK;
}

void gl_shader_prog_use_program(shader_program_t *p_src_prog)
{
	glUseProgram(p_src_prog->program);
}

int32_t gl_shader_prog_get_uniform_location(int32_t *p_dst_uniform_idx, shader_program_t *p_src_prog, const char *p_uniform_name)
{
	return (int32_t)glGetUniformLocation(p_src_prog->program, p_uniform_name);
}
