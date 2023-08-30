#include "gl_render.h"
#include <stdlib.h>
#include <stdint.h>

#include <assert.h>
#include "murmurhash.h"
#include "list.h"

#include <vector> //TEMP USE!

/* material */

/* mesh instance */
struct r_mesh_instance_t {
	int mode; //RENDER_MESH_RENDER_MODE
	int flags; //RENDER MESH INSTANCES FLAGS
	hmesh_t h_mesh;
	mat4x4 transform;
};

list_t materials_list;
list_t meshes_list;
std::vector<render_mesh_t *> meshes;

#if defined LINUX


#else

LRESULT CALLBACK wnd_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{




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
	int curr_attrib = 0;
	int context_attribs[128];
	DWORD last_error;
	HGLRC h_temp_glctx;
	PIXELFORMATDESCRIPTOR pfd;

#define push_context_attrib(name, val)\
	do {\
		context_attribs[curr_attrib++] = name;\
		context_attribs[curr_attrib++] = val;\
	} while(0);

#define end_context_attribs()\
	do {\
		context_attribs[curr_attrib++] = context_attribs[curr_attrib] = 0;\
	} while(0);

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

		h_glctx = wglCreateContextAttribsARB(h_devctx, NULL, context_attribs);
		if (!h_glctx) {
			last_error = GetLastError();
			r_nprintf(p_dst_error, dstlen, "wglCreateContextAttribsARB for main context failed! LastError() = %d (0x%x)", last_error, last_error);
			return false;
		}

		
		
		h_shared_glctx = wglCreateContextAttribsARB(h_devctx, h_glctx, context_attribs);
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

	return 0;
}

gl_render::gl_render()
{
	flags = 0;
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

int gl_render::init(char *p_dsterr, size_t maxlen, const re_render_init_info_t *p_init_info)
{
	flags |= p_init_info->flags;
	width = p_init_info->width;
	height = p_init_info->height;
	color_bits = p_init_info->color_bits;
	depth_bits = p_init_info->depth_bits;
	stencil_bits = p_init_info->stencil_bits;
#if defined LINUX

#else
	WNDCLASSEXA wcw;
	memset(&wcw, 0, sizeof(wcw));
	wcw.cbSize = sizeof(wcw);
	wcw.hbrBackground = NULL;
	wcw.hCursor = LoadCursor(NULL, MAKEINTRESOURCEW(IDC_ARROW));
	wcw.hIcon = NULL;
	wcw.hIconSm = NULL;
	wcw.hInstance = GetModuleHandleA(NULL);
	wcw.lpszClassName = WINDOW_ID;
	wcw.style = CS_HREDRAW | CS_VREDRAW;
	wcw.lpfnWndProc = wnd_proc;
	if (!RegisterClassExA(&wcw))
		return RENDER_STATUS_ERROR_INIT_WINDOW_SUBSYSTEM;

#define WINDOW_FLAGS (WS_VISIBLE|WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX)

	h_window = CreateWindowExA(0, wcw.lpszClassName, "ReactEngine GL Renderer", WINDOW_FLAGS, ));


#endif
	return 0;
}

int gl_render::shutdown()
{
	return 0;
}

int gl_render::set_window_title(const char *p_title, ...)
{
	return 0;
}

int gl_render::get_window_title(char *p_dst, size_t dstlen)
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
