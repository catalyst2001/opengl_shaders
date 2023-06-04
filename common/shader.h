#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <glad.h>

#include <assert.h>

/*
 TODO: 
 -- Add shader caching
*/

#define SHADER_CNT(a) (sizeof(a) / sizeof(a[0]))

typedef enum SHADER_PROG_STATUS_ {
	SHADER_PROG_STATUS_OK = 0,
	SHADER_PROG_STATUS_FAILED_TO_OPEN_FILE,
	SHADER_PROG_STATUS_COMPILATION_ERROR,
	SHADER_PROG_STATUS_INVALID_SHADER_TYPE,
	//SHADER_PROG_STATUS_MALFORMED_CACHE,
} SHADER_PROG_STATUS;

/* SHADER TYPE FLAGS */
#define SHADER_FLAG_NONE (0)
#define SHADER_FLAG_VERTEX (1 << 0)
#define SHADER_FLAG_FRAMGENT (1 << 1)
#define SHADER_FLAG_GEOMETRY (1 << 2)

/* SHADER PROGRAM LOAD FLAGS */
#define SHADER_PROG_COMPILE_ANYWAY (1 << 0)
#define SHADER_PROG_LOAD_FROM_GROUPPED_FILE (1 << 1)

/* SHADERS INDICES */
typedef enum SHADER_TYPE_ {
	SHADER_TYPE_VERTEX = 0,
	SHADER_TYPE_GEOMETRY,
	SHADER_TYPE_FRAGMENT,

	SHADER_TYPES_COUNT
} SHADER_TYPE;

typedef struct shader_program_s {
	uint32_t flags;
	uint32_t program;
	union {
		struct { GLuint vert_shader, geom_shader, frag_shader; };
		GLuint shaders[SHADER_TYPES_COUNT];
	};
} shader_program_t;

typedef struct shader_attrib_binding_s {
	uint32_t index;
	const char *p_attrib_name;
} shader_attrib_binding_t;

typedef struct shader_info_s {
	SHADER_TYPE type;
	const char *p_shader_source;
	shader_attrib_binding_t *p_attribs_bindings;
	uint32_t num_attribs;
} shader_info_t;

bool               gl_shader_source_compile(GLuint *p_dstshader, char *p_dsterr, size_t dstlen, GLenum shader_type, shader_info_t *p_shader_info);
SHADER_PROG_STATUS gl_shader_prog_create_program(shader_program_t *p_dst_prog, char *p_dsterr, size_t dstlen, uint32_t flags, const shader_info_t *p_shaders_infos, uint32_t shaders_count);

SHADER_PROG_STATUS gl_shader_prog_delete_program(shader_program_t *p_src_prog);
void               gl_shader_prog_use_program(shader_program_t *p_src_prog);
int32_t            gl_shader_prog_get_uniform_location(int32_t *p_dst_uniform_idx, shader_program_t *p_src_prog, const char *p_uniform_name);