#include "gl_render.h"
#include <stdlib.h>
#include <stdint.h>

#include <assert.h>
#include "murmurhash.h"
#include "list.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const char *va(const char *p_format, ...)
{
	static char buf[1024];
	va_list argptr;
	va_start(argptr, p_format);
	vsprintf_s(buf, sizeof(buf), p_format, argptr);
	va_end(argptr);
	return buf;
}

GL_SHADER_OBJECT_STATUS gl_shader_object_compile(GLuint *p_dst_object, char *p_dst_err, size_t dst_maxlen, GLenum shader_type, const char *p_text)
{
	return GL_SHADER_OBJECT_STATUS();
}

/* material */

/* mesh instance */
struct r_mesh_instance_t {
	size_t mesh_index; //for fast remove from meshes list if mesh is static
	int mode; //RENDER_MESH_RENDER_MODE
	int flags; //RENDER MESH INSTANCES FLAGS
	hmesh_t h_mesh;
	glm::mat4x4 transform;
};

render_log_message_callback log_msg;
list_t materials_list;
list_t meshes_list;
std::vector<render_mesh_t *> meshes;

#define LOG_MSG_INIT(funcptr) log_msg = funcptr;

#if defined LINUX


#else

LRESULT CALLBACK wnd_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_CLOSE: {
		PostQuitMessage(0);
		break;
	}



	default:
		return DefWindowProcW(hwnd, msg, wparam, lparam);
	}
	return 0;
}

#endif

bool gl_render::create_gl_context(char *p_dst_error, size_t dstlen)
{
#if LINUX


#else
	int pixelformat;
	DWORD last_error;
	HGLRC h_temp_glctx;
	PIXELFORMATDESCRIPTOR pfd;

	int curr_attrib = 0;
	struct context_attrib_t {
		int id;
		int value;
	} context_attribs[64];

#define push_context_attrib(attrib_id, val)\
	do {\
		context_attribs[curr_attrib].id = attrib_id;\
		context_attribs[curr_attrib].value = val;\
		curr_attrib++;\
	} while(0);

#define end_context_attribs() push_context_attrib(0, 0)

	h_devctx = GetDC(h_window);
	if (!h_devctx) {
		last_error = GetLastError();
		r_nprintf(p_dst_error, dstlen, "Failed to capture device context! LastError() = %d (0x%x)", last_error, last_error);
		return false;
	}

	memset(&pfd, 0, sizeof(pfd));
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.cColorBits = color_bits;
	pfd.cDepthBits = depth_bits;
	pfd.cStencilBits = stencil_bits;
	pfd.iLayerType = PFD_MAIN_PLANE;
	pixelformat = ChoosePixelFormat(h_devctx, &pfd);
	if (!pixelformat) {
		last_error = GetLastError();
		r_nprintf(p_dst_error, dstlen, "ChoosePixelFormat failed! LastError() = %d (0x%x)", last_error, last_error);
		return false;
	}

	if(!SetPixelFormat(h_devctx, pixelformat, &pfd)) {
		last_error = GetLastError();
		r_nprintf(p_dst_error, dstlen, "SetPixelFormat failed! LastError() = %d (0x%x)", last_error, last_error);
		return false;
	}

	/* create basic gl context */
	h_temp_glctx = wglCreateContext(h_devctx);
	if (!h_temp_glctx) {
		last_error = GetLastError();
		r_nprintf(p_dst_error, dstlen, "wglCreateContext failed! LastError() = %d (0x%x)", last_error, last_error);
		return false;
	}

	if(!wglMakeCurrent(h_devctx, h_temp_glctx)) {
		last_error = GetLastError();
		r_nprintf(p_dst_error, dstlen, "wglMakeCurrent failed! LastError() = %d (0x%x)", last_error, last_error);
		return false;
	}

	/* resolve wgl extensions */
	wglGetExtensionsStringEXT = (PFNWGLGETEXTENSIONSSTRINGEXTPROC)wglGetProcAddress("wglGetExtensionsStringEXT");
	wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
	if (wglCreateContextAttribsARB) {

		/* setup extended context */
		push_context_attrib(WGL_CONTEXT_MINOR_VERSION_ARB, MIN_GL_VERSION);
		push_context_attrib(WGL_CONTEXT_MAJOR_VERSION_ARB, MAJ_GL_VERSION);
		push_context_attrib(WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB);

		/* WGL_ARB_create_context_robustness */
		if (ext_avalible("WGL_ARB_create_context_robustness")) {
			push_context_attrib(WGL_CONTEXT_ROBUST_ACCESS_BIT_ARB, 1);
			push_context_attrib(WGL_CONTEXT_RESET_NOTIFICATION_STRATEGY_ARB, WGL_LOSE_CONTEXT_ON_RESET_ARB);
		}

		if ((flags & RENDER_INIT_FLAG_MULTISAMPLING) && ext_avalible("WGL_ARB_multisample")) {
			push_context_attrib(WGL_SAMPLE_BUFFERS_ARB, 1);
			push_context_attrib(WGL_SAMPLES_ARB, samples);
		}
		end_context_attribs();

#if 1
		printf("--- context attribs ---\n");
		for (int i = 0; i < curr_attrib; i++)
			printf("Attrib %d: %d %d\n", i, context_attribs[i].id, context_attribs[i].value);
#endif

		h_glctx = wglCreateContextAttribsARB(h_devctx, NULL, (const int *)context_attribs);
		if (!h_glctx) {
			last_error = GetLastError();
			r_nprintf(p_dst_error, dstlen, "wglCreateContextAttribsARB for main context failed! LastError() = %d (0x%x)", last_error, last_error);
			return false;
		}

		h_shared_glctx = wglCreateContextAttribsARB(h_devctx, h_glctx, (const int *)context_attribs);
		if (!h_shared_glctx) {
			last_error = GetLastError();
			r_nprintf(p_dst_error, dstlen, "wglCreateContextAttribsARB for resource context failed! LastError() = %d (0x%x)", last_error, last_error);
			return false;
		}

		if(!wglMakeCurrent(h_devctx, h_glctx)) {
			last_error = GetLastError();
			r_nprintf(p_dst_error, dstlen, "wglMakeCurrent for extended context failed! LastError() = %d (0x%x)", last_error, last_error);
			return false;
		}

		wglDeleteContext(h_temp_glctx);
	}
	else
	{
		h_glctx = h_temp_glctx;
	}

#undef end_context_attribs
#undef push_context_attrib
#endif



	return true;
}

bool gl_render::destroy_gl_context()
{
#if LINUX


#else
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(h_glctx);
	wglDeleteContext(h_shared_glctx);
	h_glctx = NULL;
	h_shared_glctx = NULL;
	ReleaseDC(h_window, h_devctx);
	h_devctx = NULL;
#endif
	return 0;
}

int gl_render::init_gl_defauls_shaders()
{
	char error[128];

	/* default 3d shader */
	union {
		GLuint vertex_obj, fragment_obj;
		GLuint objects[2];
	};

	static const char *p_vert_shader = {
		"#version 330 core\n"
		""
		""
		""
		""
		""
	};

	static const char *p_frag_shader = {
		"#version 330 core\n"
		""
		""
		""
		""
		""
	};

	gl_shader_object_compile(&vertex_obj, error, sizeof(error), GL_VERTEX_SHADER, p_vert_shader);
	gl_shader_object_compile(&fragment_obj, error, sizeof(error), GL_FRAGMENT_SHADER, p_frag_shader);
	gl_shader_program_create_and_link(&default_3d_program, error, sizeof(error), objects, 2);

	/* default 2d shader */



	return 0;
}

int gl_render::init_gl_renderer(char *p_dst_error, size_t maxlen)
{
	window_size_t size;

	LOG_NOTIFY("Initializing context...");
	if (!create_gl_context(p_dst_error, maxlen))
		return RENDER_STATUS_ERROR_CREATE_CONTEXT;

	LOG_NOTIFY("Loading extensions...");
	if (!gladLoadGL())
		return RENDER_STATUS_ERROR_NEEDED_EXTENSIONS_NO_AVALIBLE;

	/* generate vertex arrays */
	glGenVertexArrays(GL_RENDER_NUM_VAO, vao);


	get_window_size(&size);
	glViewport(0, 0, size.width, size.height);

	LOG_NOTIFY("Renderer initialized");
	return RENDER_STATUS_OK;
}

gl_render::gl_render()
{
	flags = RENDER_INIT_FLAG_DEFAULT;
	wflags = RENDER_WINDOW_FLAG_NONE;
	width = 640;
	height = 480;
	color_bits = 24;
	depth_bits = 32;
	stencil_bits = 32;
	samples = 4;
#if defined LINUX
	h_window = NULL;
#else
	h_window = NULL;
	h_devctx = NULL;
	h_glctx = NULL;
	wglGetExtensionsStringEXT = NULL;
	wglCreateContextAttribsARB = NULL;
#endif
}

gl_render::~gl_render()
{
#if defined LINUX

#else

#endif
}

int gl_render::init(char *p_dst_error, size_t maxlen, const re_render_init_info_t *p_init_info, render_log_message_callback p_log_callback)
{
	int status;
	window_size_t size;
	flags |= p_init_info->flags;
	width = p_init_info->width;
	height = p_init_info->height;
	color_bits = p_init_info->color_bits;
	depth_bits = p_init_info->depth_bits;
	stencil_bits = p_init_info->stencil_bits;

	LOG_MSG_INIT(p_log_callback)
	LOG_NOTIFY("Initializing renderer subsystem...");

#if defined LINUX

#else
	DWORD last_error;
	WNDCLASSEXA wcw;
	memset(&wcw, 0, sizeof(wcw));
	wcw.cbSize = sizeof(wcw);
	wcw.style = CS_HREDRAW|CS_VREDRAW|CS_OWNDC;
	wcw.lpszClassName = WINDOW_ID;
	wcw.hbrBackground = NULL;
	wcw.hCursor = LoadCursorW(NULL, MAKEINTRESOURCEW(IDC_ARROW));
	wcw.hIcon = NULL;
	wcw.hIconSm = NULL;
	wcw.hInstance = GetModuleHandleA(NULL);
	wcw.lpfnWndProc = wnd_proc;
	if (!RegisterClassExA(&wcw))
		return RENDER_STATUS_ERROR_INIT_WINDOW_SUBSYSTEM;

#define WINDOW_FLAGS (WS_VISIBLE|WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX)

	h_window = CreateWindowExA(0, wcw.lpszClassName, "ReactEngine GL Renderer",
		WINDOW_FLAGS,
		0, 0, width, height,
		HWND_DESKTOP,
		(HMENU)NULL,
		NULL,
		NULL);

	if (!h_window) {
		last_error = GetLastError();
		r_nprintf(p_dst_error, maxlen, "ChoosePixelFormat failed! LastError() = %d (0x%x)", last_error, last_error);
		return RENDER_STATUS_ERROR_WINDOW;
	}

	ShowWindow(h_window, SW_SHOW);
	UpdateWindow(h_window);

	LOG_NOTIFY("Initializing OpenGL renderer...");
	status = init_gl_renderer(p_dst_error, maxlen);
	if (status != RENDER_STATUS_OK)
		return status;

	wflags |= RENDER_WINDOW_FLAG_RUNNING;
#endif
	return RENDER_STATUS_OK;
}

int gl_render::shutdown()
{
	/* unloading resources */
	LOG_NOTIFY("Unloading resouces...");

	/* destroy GL context */
	LOG_NOTIFY("Destroying OpenGL context...");
	destroy_gl_context();

	/* destroy window */
	

	LOG_NOTIFY("Shutdown renderer");
	return 0;
}

gl_buffer<GL_ARRAY_BUFFER, GL_STATIC_DRAW> vertices;
gl_buffer<GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW> indices;

#include "rabdata.h"

int gl_render::render_cycle()
{
	glm::mat4x4 world_matrix = glm::mat4x4(1.f);

#if 1
	assert(vertices.alloc(rabbit_vertices, sizeof(rabbit_vertices)) == GL_BUFFER_STATUS_OK);
	assert(indices.alloc(rabbit_triangles, sizeof(rabbit_triangles)) == GL_BUFFER_STATUS_OK);

#endif


#if defined(LINUX)

#else
	MSG msg;
	while (wflags & RENDER_WINDOW_FLAG_RUNNING) {
		if (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessageA(&msg);
		}

		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		/* draw 3d graphics */
		glBindVertexArray(vao[GL_RENDER_VAO_3D]);
		{

			/* draw all static meshes */
			for (size_t i = 0; i < meshes.size(); i++) {
				//render_mesh_t *p_mesh = meshes[i];
				//p_mesh->
			}

			/* draw all models */
			for (size_t i = 0; i < meshes.size(); i++) {

			}
		}

		/* draw 2d graphics */
		glBindVertexArray(vao[GL_RENDER_VAO_2D]);
		{

		}
		


		SwapBuffers(h_devctx);
	}

#endif
	return 0;
}

int gl_render::set_window_title(const char *p_title, ...)
{
	char buf[256];
	va_list argptr;
	va_start(argptr, p_title);
	vsprintf_s(buf, sizeof(buf), p_title, argptr);
	va_end(argptr);
	return SetWindowTextA(h_window, buf);
}

int gl_render::get_window_title(char *p_dst, size_t dstlen)
{
	GetWindowTextA(h_window, p_dst, (int)dstlen);
	return 0;
}

int gl_render::get_window_size(window_size_t *p_dst_size)
{
	RECT rect;
	GetClientRect(h_window, &rect);
	p_dst_size->width = rect.right;
	p_dst_size->height = rect.bottom;
	return 0;
}

int gl_render::enable(int param)
{
	return 0;
}

int gl_render::disable(int param)
{
	return 0;
}

int gl_render::change_screen_mode(int screen_mode)
{
	return 0;
}

int gl_render::change_screen_resolution(int width, int height)
{
	return 0;
}

bool gl_render::ext_avalible(const char *p_extname)
{
	const char *p_exts_str;
#if defined LINUX

#else
	if (wglGetExtensionsStringEXT) {
		p_exts_str = wglGetExtensionsStringEXT();
		if (!p_exts_str)
			return false;
		
		return strstr(p_exts_str, p_extname) != NULL;
	}
#endif
}

int gl_render::res_list_dump(const char *p_reslist)
{
	return 0;
}

int gl_render::res_list_reload(const char *p_reslist)
{
	return 0;
}

int gl_render::mesh_create(hmesh_t *p_dst_hmesh, const char *p_name, RENDER_MESH_UPADTE_TYPE update_type, RENDER_MESH_TYPE mesh_type, RENDER_MESH_HINT hint, void *p_verts_data, unsigned int num_verts)
{
	return 0;
}

int gl_render::mesh_find(hmesh_t *p_dst_mesh, const char *p_name)
{
	return 0;
}

int gl_render::mesh_info(hmesh_t h_mesh)
{
	return 0;
}

int gl_render::mesh_update(hmesh_t h_mesh, int mesh_hint, int mesh_type, void *p_verts_data, unsigned int num_verts)
{
	return 0;
}

int gl_render::mesh_delete(hmesh_t h_mesh)
{
	return 0;
}

int gl_render::mesh_get_flags(int *p_dst_flags, hmesh_t h_mesh)
{
	return 0;
}

int gl_render::skel_create(hskl_t *p_dst_hskl, const char *p_name)
{
	return 0;
}

int gl_render::skel_load(hskl_t *p_dst_hskl, const char *p_filename)
{
	return 0;
}

int gl_render::skel_find(hskl_t *p_dst_hskl, const char *p_name)
{
	return 0;
}

int gl_render::skel_dump(hskl_t h_skl, const char *p_filename)
{
	return 0;
}

int gl_render::skel_delete(hskl_t h_skl)
{
	return 0;
}

int gl_render::anim_create(hanim_t *p_dst_hanim, const char *p_name, float fps, uint32_t num_keyframes)
{
	return 0;
}

int gl_render::anim_load(hanim_t *p_dst_hanim, const char *p_path, const char *p_name)
{
	return 0;
}

int gl_render::anim_info(anim_info_t *p_dst_info, hanim_t h_anim)
{
	return 0;
}

int gl_render::anim_dump(hanim_t h_anim, const char *p_filename)
{
	return 0;
}

int gl_render::anim_delete(hanim_t h_anim)
{
	return 0;
}

int gl_render::model_create(hmdl_t *p_dst_hmdl, const char *p_name)
{
	return 0;
}

int gl_render::model_set_base_skeleton(hmdl_t h_mdl, hskl_t h_skel)
{
	return 0;
}

int gl_render::model_add_mesh_part(hmdl_t h_mdl, hmesh_t h_mesh, unsigned int offset)
{
	return 0;
}

int gl_render::model_skel_is_compatible(hmdl_t h_mdl, hskl_t h_skel)
{
	return 0;
}

int gl_render::model_anim_select(RENDER_ANIM_SELECT_TYPE anim_op_type, hmdl_t h_mdl, hanim_t h_anim, unsigned int start_frame, unsigned int end_frame, float blend_factor)
{
	return 0;
}

int gl_render::model_anim_set_blend_factor(hmdl_t h_mdl, hanim_t h_anim, float blend_factor)
{
	return 0;
}

int gl_render::model_anim_set_playback_speed(hmdl_t h_mdl, hanim_t h_anim, float fps)
{
	return 0;
}

int gl_render::model_info(model_info_t *p_dst_info, hmdl_t h_mdl)
{
	return 0;
}

int gl_render::model_delete(hmdl_t h_mdl)
{
	return 0;
}

int gl_render::draw_list_add_mesh(hmesh_t h_mesh)
{
	return 0;
}

int gl_render::draw_list_remove_mesh(hmesh_t h_mesh)
{
	return 0;
}

int gl_render::draw_list_add_model(hmdl_t h_model)
{
	return 0;
}

int gl_render::draw_list_remove_model(hmdl_t h_model)
{
	return 0;
}

int gl_render::set_multisampling_samples(char *p_dsterr, size_t maxlen, int n_samples)
{
	if (samples == n_samples)
		return RENDER_STATUS_ERROR_PARAMETER_NOT_CHANGED;

	destroy_gl_context();
	samples = n_samples;
	if (!create_gl_context(p_dsterr, maxlen))
		return RENDER_STATUS_ERROR_CREATE_CONTEXT;

	return RENDER_STATUS_OK;
}

int gl_render::texture_load(htex_t *p_dst_texture, const texture_create_info_t *p_create_info)
{
	return 0;
}

int gl_render::texture_free(htex_t h_texture)
{
	return 0;
}

int gl_render::shader_program_create(hshader_program_t *p_dst_hshaderprog, shader_program_compile_log_t *p_dstlog, const char *p_file_path)
{
	return 0;
}

int gl_render::shader_program_create_from_mem(hshader_program_t *p_dst_hshaderprog, shader_program_compile_log_t *p_dstlog, const shader_program_sources_ptrs_t *p_sources)
{
	return 0;
}

int gl_render::shader_program_create_from_SPIRV(hshader_program_t *p_dst_hshaderprog, shader_program_compile_log_t *p_dstlog, uint8_t *p_spirv_code, size_t size)
{
	return 0;
}

int gl_render::shader_program_get_binary(uint8_t **p_dst_ptr, size_t *p_dst_size, hshader_program_t h_shaderprog)
{
	return 0;
}

int gl_render::shader_program_binary_free_ptr(uint8_t *p_ptr)
{
	return 0;
}

int gl_render::shader_program_delete(hshader_program_t h_shaderprog)
{
	return 0;
}

int gl_render::shaders_reload_all()
{
	return 0;
}

int gl_render::set_lighting_shader(hshader_program_t h_shaderprog)
{
	return 0;
}

int gl_render::set_shadow_shader(hshader_program_t h_shaderprog)
{
	return 0;
}



gl_ui_buffer::gl_ui_buffer()
{
	buffers[0] = 0;
	buffers[1] = 0;
}

gl_ui_buffer::~gl_ui_buffer()
{
}

int gl_ui_buffer::init(size_t start_vbo_size, size_t start_ibo_size, size_t start_cmd_size)
{
	gl_buffer_binding_saver<GL_ARRAY_BUFFER> vbo_binding;
	gl_buffer_binding_saver<GL_ELEMENT_ARRAY_BUFFER> ibo_binding;
	ibo_capacity = UI_BUFFER_VBO_SIZE_DEFAULT;
	vbo_capacity = UI_BUFFER_IBO_SIZE_DEFAULT;

	if (start_vbo_size)
		vbo_capacity = (GLsizei)start_vbo_size;

	if (start_ibo_size)
		ibo_capacity = (GLsizei)start_ibo_size;

	glGenBuffers(UI_BUFFERS_COUNT, buffers); //allocate buffers

	/* alloc vertex buffer */
	glBindBuffer(buffers[UI_BUFFER_VBO], GL_ARRAY_BUFFER);
	GL_CALL(glBufferData(GL_ARRAY_BUFFER, vbo_capacity * sizeof(ui_mesh_vertex_t), NULL, GL_DYNAMIC_DRAW));

	/* alloc index buffer */
	glBindBuffer(buffers[UI_BUFFER_IBO], GL_ELEMENT_ARRAY_BUFFER);
	GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, ibo_capacity * sizeof(unsigned int), NULL, GL_DYNAMIC_DRAW));
	return 0;
}

int gl_ui_buffer::shutdown()
{
	clear();
	glDeleteBuffers(UI_BUFFERS_COUNT, buffers);
	return 0;
}

void gl_ui_buffer::clear()
{
	indices.clear();
	vertices.clear();
	drawcmds.clear();
}

bool gl_ui_buffer::commit()
{
	if (ibo_capacity < (GLsizeiptr)indices.size()) {}
	//TODO: continue


	return false;
}
