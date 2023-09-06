#pragma once
#include <vector> //TEMP USE!
#include "ire_render.h"

#define WINDOW_ID "rge_window"

#define MIN_GL_VERSION 3
#define MAJ_GL_VERSION 3

#if defined(_MSC_VER) && !defined(__PRETTY_FUNCTION__)
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

extern render_log_message_callback log_msg;

#if defined LINUX
#include <x11.h>

#define r_snprintf(b, s, f, ...) sprintf(b, f, __VA_ARGS__)

#else
#include <Windows.h>
#include "glad/glad.h"
#include "GL/wglext.h"

#pragma comment(lib, "opengl32.lib")

#define r_nprintf(b, s, f, ...) sprintf_s(b, s, f, __VA_ARGS__)

#endif

/* STRING FORMAT */
const char *va(const char *p_format, ...);

#if defined(GL_DEBUG_MODE) || defined(_DEBUG)
#define GL_CALL(func)\
	func;\
	if(glGetError() != GL_NO_ERROR) {\
		log_msg(RENDER_LOG_MSG_CRITICAL_ERROR, va("OpenGL Error: " __PRETTY_FUNCTION__ " Call " #func " failed with error %d!\n", glGetError())); \
	}

#define GL_CHECK(err, func) if(err != GL_NO_ERROR) { log_msg(RENDER_LOG_MSG_CRITICAL_ERROR, va("OpenGL Error: " __PRETTY_FUNCTION__ " Call " func "failed with error %d!\n", err)); }

#else
#define GL_CALL(func) func;
#define GL_CHECK(err, func)
#endif

/* LOG MACRO */
#define LOG_NOTIFY(format, ...) log_msg(RENDER_LOG_MSG_NOTIFY, va(format, __VA_ARGS__))
#define LOG_WARNING(format, ...) log_msg(RENDER_LOG_MSG_WARNING, va(format, __VA_ARGS__))
#define LOG_ERROR(format, ...) log_msg(RENDER_LOG_MSG_CRITICAL_ERROR, va(format, __VA_ARGS__))

/* GL BUFFERS BINDING SAVER CLASS FOR CORE PROFILE */
template<GLint binding>
constexpr GLint get_target_from_binding_name()
{
	static_assert(binding != GL_ARRAY_BUFFER_BINDING ||
		binding != GL_PIXEL_PACK_BUFFER_BINDING ||
		binding != GL_ELEMENT_ARRAY_BUFFER_BINDING ||
		binding != GL_TEXTURE_BINDING_1D ||
		binding != GL_TEXTURE_BINDING_2D ||
		binding != GL_TEXTURE_BINDING_3D ||
		binding != GL_TEXTURE_BINDING_CUBE_MAP, __FUNCSIG__ ": Failed to find specified binding!");

	if (binding == GL_ARRAY_BUFFER_BINDING)
		return GL_ARRAY_BUFFER;

	if (binding == GL_PIXEL_PACK_BUFFER_BINDING)
		return GL_PIXEL_PACK_BUFFER;

	if (binding == GL_ELEMENT_ARRAY_BUFFER_BINDING)
		return GL_ELEMENT_ARRAY_BUFFER;

	if (binding == GL_TEXTURE_BINDING_1D)
		return GL_TEXTURE_1D;

	if (binding == GL_TEXTURE_BINDING_2D)
		return GL_TEXTURE_2D;

	if (binding == GL_TEXTURE_BINDING_3D)
		return GL_TEXTURE_3D;

	if (binding == GL_TEXTURE_BINDING_CUBE_MAP)
		return GL_TEXTURE_CUBE_MAP;

	return 0; // unreachable code. used for syntax correct
}

template<GLint target>
constexpr GLint get_binding_name_from_target()
{
	static_assert(target != GL_ARRAY_BUFFER ||
		target != GL_PIXEL_PACK_BUFFER ||
		target != GL_ELEMENT_ARRAY_BUFFER ||
		target != GL_TEXTURE_1D ||
		target != GL_TEXTURE_2D ||
		target != GL_TEXTURE_3D ||
		target != GL_TEXTURE_CUBE_MAP, __FUNCSIG__ ": Failed to find specified target!");

	if (target == GL_ARRAY_BUFFER)
		return GL_ARRAY_BUFFER_BINDING;

	if (target == GL_PIXEL_PACK_BUFFER)
		return GL_PIXEL_PACK_BUFFER_BINDING;

	if (target == GL_ELEMENT_ARRAY_BUFFER)
		return GL_ELEMENT_ARRAY_BUFFER_BINDING;

	if (target == GL_TEXTURE_1D)
		return GL_TEXTURE_BINDING_1D;

	if (target == GL_TEXTURE_2D)
		return GL_TEXTURE_BINDING_2D;

	if (target == GL_TEXTURE_3D)
		return GL_TEXTURE_BINDING_3D;

	if (target == GL_TEXTURE_CUBE_MAP)
		return GL_TEXTURE_BINDING_CUBE_MAP;

	return 0; // unreachable code. used for syntax correct
}

template<GLint target>
class gl_buffer_binding_saver
{
	GLint previous_name;
public:
	gl_buffer_binding_saver() { glGetIntegerv(get_binding_name_from_target<target>(), &previous_name); }
	~gl_buffer_binding_saver() { glBindBuffer(target, previous_name); }
};

/* GL UI 2D MESH */
class gl_ui_buffer
{
	enum UI_GL_BUFFER {
		UI_BUFFER_VBO = 0,
		UI_BUFFER_IBO,

		UI_BUFFERS_COUNT
	};

	enum UI_GL_BUFFER_DEFAULS {
		UI_BUFFER_VBO_SIZE_DEFAULT = 1024,
		UI_BUFFER_IBO_SIZE_DEFAULT = 1024
	};

	GLsizeiptr ibo_capacity;
	GLsizeiptr vbo_capacity;
	GLuint buffers[UI_BUFFERS_COUNT];
	std::vector<unsigned int> indices;
	std::vector<ui_mesh_vertex_t> vertices;
	std::vector<mesh_draw_command_t> drawcmds;
public:
	gl_ui_buffer();
	~gl_ui_buffer();

	int init(size_t start_vbo_size = 0, size_t start_ibo_size = 0, size_t start_cmd_size = 0);
	int shutdown();

	/* clear temp vertex buffer*/
	void clear();

	/* commit temp vertices data to video memory */
	bool commit();

	void adjust_buffer_size();

	/* push geometry */
	uint32_t push_vertex(ui_mesh_vertex_t *p_verts, uint32_t count);
	uint32_t push_triangle(ui_mesh_vertex_t *p_verts);
	uint32_t push_rect(ui_mesh_vertex_t *p_verts);

	/* update vertices */
	bool update(uint32_t start_index, ui_mesh_vertex_t *p_vertices, uint32_t count_indices);
};

/* GL SHADER OBJECT */
enum GL_SHADER_OBJECT_STATUS {
	GL_SHADER_OBJECT_STATUS_OK = 0,
	GL_SHADER_OBJECT_STATUS_SYNTAX_ERROR
};

GL_SHADER_OBJECT_STATUS gl_shader_object_compile(GLuint *p_dst_object, char *p_dst_err, size_t dst_maxlen, GLenum shader_type, const char *p_text);
GL_SHADER_OBJECT_STATUS gl_shader_object_delete(GLuint src_object);

/* GL PROGRAM OBJECT */
enum GL_SHADER_PROGRAM_STATUS {
	GL_SHADER_PROGRAM_STATUS_OK = 0,
	GL_SHADER_PROGRAM_STATUS_LINK_ERROR
};

GL_SHADER_PROGRAM_STATUS gl_shader_program_create_and_link(GLuint *p_dst_object, char *p_dst_err, size_t dst_maxlen, const GLuint *p_objects, GLuint num_objects);
GL_SHADER_PROGRAM_STATUS gl_shader_program_validate(GLuint src_object, char *p_dst_err, size_t dst_maxlen);
GL_SHADER_PROGRAM_STATUS gl_shader_program_delete(GLuint src_object);

/* GL BUFFER */
enum GL_BUFFER_STATUS {
	GL_BUFFER_STATUS_OK = 0,
	GL_BUFFER_STATUS_OUT_OF_MEMORY,
	GL_BUFFER_STATUS_NOT_ALLOCATED,
	GL_BUFFER_STATUS_COPY_ERROR,
	GL_BUFFER_STATUS_SIZE_ERROR,
	GL_BUFFER_STATUS_DELETE_ERROR,
	GL_BUFFER_STATUS_MAP_ERROR,
	GL_BUFFER_STATUS_INVALID_PARAM,
};

template<GLenum _target, GLenum _access>
class gl_buffer
{
	GLuint name;

	bool buffer_alloc(GLuint *p_dst_name, GLsizeiptr size, const void *p_data) {
		GLenum error;
		gl_buffer_binding_saver<_target> bind_save;
		glGenBuffers(1, p_dst_name);
		glBindBuffer(_target, *p_dst_name);
		glBufferData(_target, size, p_data, _access);
		error = glGetError();
		GL_CHECK(error, "glBufferData")
		return error == GL_NO_ERROR;
	}

	bool buffer_size(GLsizeiptr *p_dst, GLuint name) {
		GLenum error;
		gl_buffer_binding_saver<_target> target_bind_save;
		glBindBuffer(_target, name);
		glGetBufferParameteriv(_target, GL_BUFFER_SIZE, (GLint *)p_dst);
		error = glGetError();
		GL_CHECK(error, "glGetBufferParameteriv");
		return error == GL_NO_ERROR;
	}

	bool buffer_copy_data(GLint dst_name, GLint src_name, GLint read_offs, GLint write_offs, GLint size) {		
		/*
		FIXME: get previous GL_COPY_WRITE_BUFFER and GL_COPY_READ_BUFFER extensions avalible on GL 4.1
		Currently min support GL 3.3
		*/
		
		GLenum error;
		glBindBuffer(GL_COPY_WRITE_BUFFER, dst_name);
		glBindBuffer(GL_COPY_READ_BUFFER, src_name);
		glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, read_offs, write_offs, size);
		error = glGetError();
		GL_CHECK(error, "glCopyBufferSubData")
		return error == GL_NO_ERROR;
	}

	bool buffer_delete(GLuint name) {
		GLenum error;
		glDeleteBuffers(1, &name);
		error = glGetError();
		GL_CHECK(error, "glDeleteBuffers")
		return error == GL_NO_ERROR;
	}

	GL_BUFFER_STATUS buffer_realloc(GLuint *p_dst_name, GLsizeiptr newsize) {
		GLuint buffer;
		GLuint src_copy_name = *p_dst_name;
		GLsizeiptr curr_buffer_size;
		GLsizeiptr new_buffer_size;

		if (!buffer_size(&curr_buffer_size, src_copy_name))
			return GL_BUFFER_STATUS_SIZE_ERROR;

		/* adjust new buffer size */
		new_buffer_size = curr_buffer_size;
		if (newsize > curr_buffer_size)
			new_buffer_size = newsize;

		/* allocate new buffer */
		if (!buffer_alloc(&buffer, new_buffer_size, NULL))
			return GL_BUFFER_STATUS_OUT_OF_MEMORY;

		/* copy data from old buffer to new buffer */
		if (!buffer_copy_data(buffer, src_copy_name, 0, 0, curr_buffer_size))
			return GL_BUFFER_STATUS_COPY_ERROR;

		/* free old buffer */
		if ((*p_dst_name) != 0 && !buffer_delete((*p_dst_name)))
			return GL_BUFFER_STATUS_DELETE_ERROR;

		*p_dst_name = buffer;
		return GL_BUFFER_STATUS_OK;
	}

public:
	gl_buffer() : name(0) {}
	~gl_buffer() {}

	void get_size(size_t *p_dst_size) {
		*p_dst_size = 0;
		GL_CALL(glGetBufferParameteriv(_target, GL_BUFFER_SIZE, (GLint *)p_dst_size))
	}

	bool is_allocated() { return name != 0; }

	GL_BUFFER_STATUS alloc(const void *p_data, size_t size) {
		/* allocate buffer */
		if (!buffer_alloc(&name, (GLsizeiptr)size, p_data))
			return GL_BUFFER_STATUS_OUT_OF_MEMORY;

		return GL_BUFFER_STATUS_OK;
	}

	GL_BUFFER_STATUS realloc(const void *p_data, size_t size) {

		/* check for last allocations */
		if (!is_allocated())
			return alloc(p_data, size);

		/* allocate new buffer */
		return buffer_realloc(&name, size);
	}

	GL_BUFFER_STATUS copy_to(char *p_dst, size_t dstlen) {
		GLenum error;
		GLsizeiptr src_size;
		if (!buffer_size(&src_size, name))
			return GL_BUFFER_STATUS_SIZE_ERROR;

		const GLubyte *p_data = glMapBuffer(_target, GL_READ_ONLY);
		error = glGetError();
		if (!p_data) {
			GL_CHECK(error, "glMapBuffer")
			return GL_BUFFER_STATUS_MAP_ERROR;
		}
		memcpy_s(p_dst, dstlen, p_data, (size_t)src_size);
		glUnmapBuffer(_target);
		return GL_BUFFER_STATUS_OK;
	}

	GL_BUFFER_STATUS free() {
		GL_BUFFER_STATUS status = buffer_delete(name);
		name = 0;
		return status;
	}
};

/* MAIN VIDEO DEVICES QUERY INFO CLASS */
class sys_visual_info : public ire_visual_info
{
public:


};

/* MAIN RENDER CLASS */
class gl_render : public ire_render
{
	int flags;
	int wflags;
	int width;
	int height;
	int color_bits;
	int depth_bits;
	int stencil_bits;
	int samples;

	enum GL_RENDER_VAO {
		GL_RENDER_VAO_2D = 0,
		GL_RENDER_VAO_3D,

		GL_RENDER_NUM_VAO
	};

	/* gl resources */
	GLuint vao[GL_RENDER_NUM_VAO];

	gl_buffer<GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW> ui_vbo;
	gl_buffer<GL_ELEMENT_ARRAY_BUFFER, GL_DYNAMIC_DRAW> ui_ibo;

#if defined LINUX

#else
	HWND h_window;
	HDC h_devctx;
	HGLRC h_glctx;
	HGLRC h_shared_glctx; // for multithreaded uploading resources
	PFNWGLGETEXTENSIONSSTRINGEXTPROC wglGetExtensionsStringEXT; // WGL_EXT_extensions_string
	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB; // WGL_ARB_create_context
#endif

	bool create_gl_context(char *p_dst_error, size_t dstlen);
	bool destroy_gl_context();

	int init_gl_renderer(char *p_dst_error, size_t maxlen);

public:
	gl_render();
	~gl_render();

	/* render init/shutdown */
	virtual int init(char *p_dsterr, size_t maxlen, const re_render_init_info_t *p_init_info, render_log_message_callback p_log_callback);
	virtual int shutdown();
	virtual int render_cycle();

	virtual int set_window_title(const char *p_title, ...);
	virtual int get_window_title(char *p_dst, size_t dstlen);
	virtual int get_window_size(window_size_t *p_dst_size);

	/* settings */
	virtual int enable(int param);
	virtual int disable(int param);

	virtual int change_screen_mode(int screen_mode);
	virtual int change_screen_resolution(int width, int height);

	virtual int set_multisampling_samples(char *p_dsterr, size_t maxlen, int n_samples);

	/* textures (draft 1) */
	virtual int texture_load(htex_t *p_dst_texture, const texture_create_info_t *p_create_info);
	virtual int texture_free(htex_t h_texture);

	/* shaders (draft 1) */
	virtual int shader_program_create(hshader_program_t *p_dst_hshaderprog, shader_program_compile_log_t *p_dstlog, const char *p_file_path);
	virtual int shader_program_create_from_mem(hshader_program_t *p_dst_hshaderprog, shader_program_compile_log_t *p_dstlog, const shader_program_sources_ptrs_t *p_sources);
	virtual int shader_program_create_from_SPIRV(hshader_program_t *p_dst_hshaderprog, shader_program_compile_log_t *p_dstlog, uint8_t *p_spirv_code, size_t size);
	virtual int shader_program_get_binary(uint8_t **p_dst_ptr, size_t *p_dst_size, hshader_program_t h_shaderprog);
	virtual int shader_program_binary_free_ptr(uint8_t *p_ptr);
	virtual int shader_program_delete(hshader_program_t h_shaderprog);
	virtual int shaders_reload_all();

	virtual int set_lighting_shader(hshader_program_t h_shaderprog);
	virtual int set_shadow_shader(hshader_program_t h_shaderprog);

	/* extensions */
	virtual bool ext_avalible(const char *p_extname);

	/* save loaded resources for restore context if it is loss */
	virtual int res_list_dump(const char *p_reslist);
	virtual int res_list_reload(const char *p_reslist);

	/* mesh (draft 1) */
	virtual int mesh_create(hmesh_t *p_dst_hmesh, const char *p_name, RENDER_MESH_UPADTE_TYPE update_type, RENDER_MESH_TYPE mesh_type, RENDER_MESH_HINT hint, void *p_verts_data, unsigned int num_verts);
	virtual int mesh_find(hmesh_t *p_dst_mesh, const char *p_name);
	virtual int mesh_info(hmesh_t h_mesh);
	virtual int mesh_update(hmesh_t h_mesh, int mesh_hint, int mesh_type, void *p_verts_data, unsigned int num_verts);
	virtual int mesh_delete(hmesh_t h_mesh);
	virtual int mesh_get_flags(int *p_dst_flags, hmesh_t h_mesh);

	/* skeleton (draft 1) */
	virtual int skel_create(hskl_t *p_dst_hskl, const char *p_name);
	virtual int skel_load(hskl_t *p_dst_hskl, const char *p_filename);
	virtual int skel_find(hskl_t *p_dst_hskl, const char *p_name);
	virtual int skel_dump(hskl_t h_skl, const char *p_filename);
	virtual int skel_delete(hskl_t h_skl);

	/* animation (draft 1)*/
	virtual int anim_create(hanim_t *p_dst_hanim, const char *p_name, float fps, uint32_t num_keyframes);
	virtual int anim_load(hanim_t *p_dst_hanim, const char *p_path, const char *p_name);
	virtual int anim_info(anim_info_t *p_dst_info, hanim_t h_anim);
	virtual int anim_dump(hanim_t h_anim, const char *p_filename);
	virtual int anim_delete(hanim_t h_anim);

	/* model (draft 1) */
	virtual int model_create(hmdl_t *p_dst_hmdl, const char *p_name);
	virtual int model_set_base_skeleton(hmdl_t h_mdl, hskl_t h_skel);
	virtual int model_add_mesh_part(hmdl_t h_mdl, hmesh_t h_mesh, unsigned int offset);
	virtual int model_skel_is_compatible(hmdl_t h_mdl, hskl_t h_skel);
	virtual int model_anim_select(RENDER_ANIM_SELECT_TYPE anim_op_type, hmdl_t h_mdl, hanim_t h_anim, unsigned int start_frame, unsigned int end_frame, float blend_factor);
	virtual int model_anim_set_blend_factor(hmdl_t h_mdl, hanim_t h_anim, float blend_factor);
	virtual int model_anim_set_playback_speed(hmdl_t h_mdl, hanim_t h_anim, float fps);
	virtual int model_info(model_info_t *p_dst_info, hmdl_t h_mdl);
	virtual int model_delete(hmdl_t h_mdl);
};