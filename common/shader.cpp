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

bool gl_shader_source_compile_object(uint32_t *p_dstshader, char *p_dsterr, size_t dstlen, GLenum shader_type, const char *p_source)
{
	GLenum error;
	GLsizei log_length;
	GLint compile_status;
	GLuint shader = glCreateShader(shader_type);
	GL_CALL(glCreateShader)
	if (shader == 0) {
		error = glGetError();
		sprintf_s(p_dsterr, dstlen, "glCreateShader() error %d (0x%X)", error, error);
		return false;
	}

	/* set source and compile shader */
	GL_CALL(glShaderSource(shader, 1, &p_source, NULL));
	GL_CALL(glCompileShader(shader));
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);
	if (compile_status != GL_TRUE) {
		glGetShaderInfoLog(shader, (GLsizei)dstlen, &log_length, p_dsterr);

		//char log[1024] = {0};
		//glGetShaderInfoLog(shader, sizeof(log), &log_length, log);
		//printf("log length: %d  %s\n", log_length, log);

		glDeleteShader(shader);
		return false;
	}
	*p_dstshader = (uint32_t)shader;
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

	/* create shader program and attach shaders */
	p_dst_prog->num_shaders = shaders_count;
	p_dst_prog->program = glCreateProgram();
	GL_CHECK_ERROR()
	for (i = 0; i < p_dst_prog->num_shaders; i++)
		GL_CALL(glAttachShader(p_dst_prog->program, p_shaders[i]));

	GL_CALL(glLinkProgram(p_dst_prog->program));
	glGetProgramiv(p_dst_prog->program, GL_LINK_STATUS, &program_status);
	if (program_status == GL_FALSE) {
		int InfoLogLength = 0;
		glGetProgramiv(p_dst_prog->program, GL_INFO_LOG_LENGTH, &InfoLogLength);
		glGetProgramInfoLog(p_dst_prog->program, dstlen, NULL, p_dsterr);
		glDeleteProgram(p_dst_prog->program);
		return SHADER_PROG_STATUS_LINKING_ERROR;
	}

	/* detach shaders */
	for (i = 0; i < p_dst_prog->num_shaders; i++)
		GL_CALL(glDetachShader(p_dst_prog->program, p_shaders[i]));

	/* validate shader program */
	GL_CALL(glValidateProgram(p_dst_prog->program));
	glGetProgramiv(p_dst_prog->program, GL_VALIDATE_STATUS, &program_status);
	if (program_status == GL_FALSE) {
		glGetProgramInfoLog(p_dst_prog->program, dstlen, NULL, p_dsterr);
		glDeleteProgram(p_dst_prog->program);
		return SHADER_PROG_STATUS_VALIDATE_ERROR;
	}
	return SHADER_PROG_STATUS_OK; /* finish OK! */
}

SHADER_PROG_STATUS gl_shader_prog_delete_program(shader_program_t *p_src_prog)
{
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
