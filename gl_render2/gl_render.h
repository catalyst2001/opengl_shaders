#pragma once
#include "ire_render.h"

#define WINDOW_ID "rge_window"

#define MIN_GL_VERSION 3
#define MAJ_GL_VERSION 3

#if defined LINUX
#include <x11.h>

#define r_snprintf(b, s, f, ...) sprintf(b, f, __VA_ARGS__)

#else
#include <Windows.h>
#include <gl/GL.h>
#include <gl/glu.h>
#include "GL/wglext.h"
#include "glad/glad.h"

#define r_nprintf(b, s, f, ...) sprintf_s(b, s, f, __VA_ARGS__)

#endif

#if defined(GL_DEBUG_MODE) || defined(_DEBUG)
#define GL_CALL(func)\
	func;\
	if(glGetError() != GL_NO_ERROR)\
		printf("[GL_DEBUG_MODE] Call " #func " failed with error %d!\n", glGetError());

#else

#endif

const char *va(const char *p_format, ...);

/* GL BUFFERS BINDING SAVER CLASS FOR CORE PROFILE */
template<GLint binding>
constexpr GLint get_target_from_binding_name()
{
	if constexpr (binding == GL_ARRAY_BUFFER_BINDING)
		return GL_ARRAY_BUFFER;
	
	if constexpr (binding == GL_PIXEL_PACK_BUFFER_BINDING)
		return GL_PIXEL_PACK_BUFFER;

	if constexpr (binding == GL_ELEMENT_ARRAY_BUFFER_BINDING)
		return GL_ELEMENT_ARRAY_BUFFER;

	if constexpr (binding == GL_TEXTURE_BINDING_1D)
		return GL_TEXTURE_1D;

	if constexpr (binding == GL_TEXTURE_BINDING_2D)
		return GL_TEXTURE_2D;

	if constexpr (binding == GL_TEXTURE_BINDING_3D)
		return GL_TEXTURE_3D;

	if constexpr (binding == GL_TEXTURE_BINDING_CUBE_MAP)
		return GL_TEXTURE_CUBE_MAP;

	static_assert(false, __FUNCSIG__ ": Failed to find specified binding!");
	return 0;
}

template<GLint target>
constexpr GLint get_binding_name_from_target()
{
	if constexpr (target == GL_ARRAY_BUFFER)
		return GL_ARRAY_BUFFER_BINDING;

	if constexpr (target == GL_PIXEL_PACK_BUFFER)
		return GL_PIXEL_PACK_BUFFER_BINDING;

	if constexpr (target == GL_ELEMENT_ARRAY_BUFFER)
		return GL_ELEMENT_ARRAY_BUFFER_BINDING;

	if constexpr (target == GL_TEXTURE_1D)
		return GL_TEXTURE_BINDING_1D;

	if constexpr (target == GL_TEXTURE_2D)
		return GL_TEXTURE_BINDING_2D;

	if constexpr (target == GL_TEXTURE_3D)
		return GL_TEXTURE_BINDING_3D;

	if constexpr (target == GL_TEXTURE_CUBE_MAP)
		return GL_TEXTURE_BINDING_CUBE_MAP;

	static_assert(false, __FUNCSIG__ ": Failed to find specified target!");
	return 0;
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

	/* push geometry */
	void push_vertex(ui_mesh_vertex_t *p_verts, uint32_t count);
	void push_triangle(ui_mesh_vertex_t *p_verts);
	void push_rect(ui_mesh_vertex_t *p_verts);

	/* update vertices */
	bool update(uint32_t start_index, ui_mesh_vertex_t *p_vertices, uint32_t count_indices);
};

void UTIL_draw_ui_buffer(const gl_ui_buffer &ui_vertex_buffer);

class gl_text_buffer
{
public:


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

	/* gl resources */
	GLint vao_2d, vao_3d;
	GLint ui_vbo, ui_ibo;

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
	virtual int init(char *p_dsterr, size_t maxlen, const re_render_init_info_t *p_init_info);
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
	virtual int model_add_mesh_part(hmesh_t h_mesh, unsigned int offset);
	virtual int model_skel_is_compatible(hmdl_t h_mdl, hskl_t h_skel);
	virtual int model_anim_select(RENDER_ANIM_SELECT_TYPE anim_op_type, hmdl_t h_mdl, hanim_t h_anim, unsigned int start_frame, unsigned int end_frame, float blend_factor);
	virtual int model_anim_set_blend_factor(hmdl_t h_mdl, hanim_t h_anim, float blend_factor);
	virtual int model_anim_set_playback_speed(hmdl_t h_mdl, hanim_t h_anim, float fps);
	virtual int model_info(model_info_t *p_dst_info, hmdl_t h_mdl);
	virtual int model_delete(hmdl_t h_mdl);
};