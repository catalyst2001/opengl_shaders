#include <stdio.h>
#include "r_gl_errors.h"

#include "shader.h"

/* RENDERER SHADER TYPE TO GL SHADER TYPE CONST  */
GLuint gl_shader_type_to_gl_const(SHADER_TYPE shader_type)
{
	static GLuint shader_types[] = { GL_VERTEX_SHADER, GL_GEOMETRY_SHADER, GL_FRAGMENT_SHADER };
	if (shader_type < 0 || shader_type >= (sizeof(shader_types) / sizeof(shader_types[0])))
		return GL_NONE;

	return shader_types[shader_type];
}

bool gl_shader_source_compile_object(GLuint *p_dstshader, char *p_dsterr, size_t dstlen, GLenum shader_type, shader_info_t *p_shader_info)
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
	GL_CALL(glShaderSource(shader, 1, &p_shader_info->p_shader_source, NULL));
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

bool gl_shader_delete_object(GLuint shader_object)
{
	return false;
}

SHADER_PROG_STATUS gl_shader_prog_create_program(shader_program_t *p_dst_prog, char *p_dsterr, size_t dstlen, uint32_t flags, uint32_t *p_shaders, uint32_t shaders_count)
{
	uint32_t i;
	GLint program_status;
	SHADER_PROG_STATUS status = SHADER_PROG_STATUS_OK;

	/* create shader program and attach shaders */
	p_dst_prog->num_shaders = shaders_count;
	p_dst_prog->program = glCreateProgram();
	for (size_t i = 0; i < p_dst_prog->num_shaders; i++)
		glAttachShader(p_dst_prog->program, p_shaders[i]);

	glLinkProgram(p_dst_prog->program);
	glGetProgramiv(p_dst_prog->program, GL_LINK_STATUS, &program_status);
	if (GL_TRUE != program_status) {
		status = SHADER_PROG_STATUS_LINKING_ERROR;
		goto __error_cleanup;
	}

	/* validate shader program */
	glValidateProgram(p_dst_prog->program);
	glGetProgramiv(p_dst_prog->program, GL_VALIDATE_STATUS, &program_status);
	if (GL_TRUE != program_status) {
		glGetProgramInfoLog(p_dst_prog->program, dstlen, NULL, p_dsterr);
		status = SHADER_PROG_STATUS_VALIDATE_ERROR;
		goto __error_cleanup;
	}

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
