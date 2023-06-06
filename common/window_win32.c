#include "window_win32.h"

LRESULT CALLBACK wnd_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

int r_window_create(r_window_t *p_dst_window, const r_window_init_data_t *p_cdata)
{
	int pf;
	RECT rect;
	WNDCLASSA wc;
	DWORD dw_style, dw_style_ex;
	PIXELFORMATDESCRIPTOR pfd;
	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;
	window_win32_t *p_window = (window_win32_t *)calloc(1, sizeof(window_win32_t));
	if (!p_window)
		return RSTATUS_MEM_ALLOC_FAILED;
	
	memset(&wc, 0, sizeof(wc));
	wc.style = CS_HREDRAW|CS_VREDRAW|CS_OWNDC;
	wc.hInstance = GetModuleHandleW(0);
	p_window->p_classname = wc.lpszClassName = (p_cdata->p_wclass) ? p_cdata->p_wclass : RW_DEFAULT_WCLASS;
	wc.hCursor = LoadCursorW(NULL, MAKEINTRESOURCEW(IDC_ARROW));
	wc.hIcon = LoadIconW(NULL, MAKEINTRESOURCEW(IDI_APPLICATION));
	wc.lpfnWndProc = wnd_proc;
	if (!RegisterClassA(&wc)) {
		r_window_destroy_window(TO_WINDOW_HANDLE(p_window));
		return RSTATUS_WINDOW_ERROR;
	}

	/* create window */
	p_window->h_window = CreateWindowExA(0, p_window->p_classname, p_cdata->p_title,
		WS_OVERLAPPEDWINDOW, 0, 0, RW_DEFAULT_WIDTH, RW_DEFAULT_HEIGHT, HWND_DESKTOP, (HMENU)NULL, NULL, NULL);
	if (!p_window->h_window) {
		r_window_destroy_window(TO_WINDOW_HANDLE(p_window));
		return RSTATUS_WINDOW_ERROR;
	}

	/* capture device context */
	p_window->h_dev_ctx = GetDC(p_window->h_window);
	if (!p_window->h_dev_ctx) {
		r_window_destroy_window(TO_WINDOW_HANDLE(p_window));
		return RSTATUS_CONTEXT_ERROR;
	}

	/* choose visual */
	memset(&pfd, 0, sizeof(pfd));
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = p_cdata->color_bits;
	pfd.cDepthBits = p_cdata->depth_bits;
	pfd.cStencilBits = 8;
	pf = ChoosePixelFormat(p_window->h_dev_ctx, &pfd);
	if (!pf || !SetPixelFormat(p_window->h_dev_ctx, pf, &pfd)) {
		r_window_destroy_window(TO_WINDOW_HANDLE(p_window));
		return RSTATUS_CHOOSE_VISUAL_ERROR;
	}

	/* create gl context */
	p_window->h_gl_ctx = wglCreateContext(p_window->h_dev_ctx);
	if (!p_window->h_gl_ctx) {
		r_window_destroy_window(TO_WINDOW_HANDLE(p_window));
		return RSTATUS_CONTEXT_ERROR;
	}

	/* select context */
	if (!wglMakeCurrent(p_window->h_dev_ctx, p_window->h_gl_ctx)) {
		r_window_destroy_window(TO_WINDOW_HANDLE(p_window));
		return RSTATUS_MAKE_CURRENT_ERROR;
	}

	wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
	
	/* new extended context avalible? */
	if (wglCreateContextAttribsARB) {

		/* free old gl context */
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(p_window->h_gl_ctx);

		/* create new extended gl context */
		int ex_attribs[] = {
			WGL_CONTEXT_MAJOR_VERSION_ARB, RW_MIN_GL_CONTEXT_VER_MAJOR, //2
			WGL_CONTEXT_MINOR_VERSION_ARB, RW_MIN_GL_CONTEXT_VER_MINOR, //0
			WGL_CONTEXT_FLAGS_ARB,         WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
			WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
			0
		};

		p_window->h_gl_ctx = wglCreateContextAttribsARB(p_window->h_dev_ctx, NULL, ex_attribs);
		if (!p_window->h_gl_ctx) {
			r_window_destroy_window(TO_WINDOW_HANDLE(p_window));
			return RSTATUS_CONTEXT_ERROR;
		}

		/* select extended context */
		if (!wglMakeCurrent(p_window->h_dev_ctx, p_window->h_gl_ctx)) {
			r_window_destroy_window(TO_WINDOW_HANDLE(p_window));
			return RSTATUS_MAKE_CURRENT_ERROR;
		}

		// OK!
	}


	//TODO: change window mode


	p_dst_window->ptr = (void *)p_window;
	return RSTATUS_OK;
}

void r_window_destroy_window(r_window_t h_window)
{
	window_win32_t *p_window = (window_win32_t *)h_window.ptr;
	if (p_window) {
		if (p_window->h_gl_ctx) {
			wglMakeCurrent(NULL, NULL);
			wglDeleteContext(p_window->h_gl_ctx);
			p_window->h_gl_ctx = NULL;
		}

		if (p_window->h_dev_ctx) {
			ReleaseDC(p_window->h_window, p_window->h_dev_ctx);
			p_window->h_dev_ctx = NULL;
		}

		if (p_window->h_window) {
			DestroyWindow(p_window->h_window);
			p_window->h_window = NULL;
		}

		if (p_window->p_classname) {
			UnregisterClassA(p_window->p_classname, GetModuleHandleW(0));
			p_window->p_classname = NULL;
		}
		free(p_window);
	}
}

bool r_window_change_mode(r_window_t h_window, const char *p_title, WINDOW_CHANGE_MODE_FLAGS flags, WINDOW_MODES mode, int width, int height)
{
	switch (mode) {
	case WINDOW_MODE_WINDOWED: {

		break;
	}

	case WINDOW_MODE_RECT: //TODO: UNUSED
	case WINDOW_MODE_FULLSCREEN: {

		break;
	}

	}

	//TODO: continue


	return false;
}

bool r_window_destroy(r_window_t h_window)
{
	return false;
}

void r_window_swap_buffers(r_window_t h_window)
{
	SwapBuffers(((window_win32_t *)h_window.ptr)->h_dev_ctx);
}

void r_window_get_info(r_window_info_t *p_dst, r_window_t h_window)
{

	//TODO: continue
}

LRESULT CALLBACK wnd_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg) {

		//TODO: add event handling

	default:
		return DefWindowProcA(hwnd, msg, wparam, lparam);
	}
	return 0;
}
