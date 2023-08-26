#include "gl_window.h"
#include "GL/wglext.h"

#define GLW_FLAG_NONE        (0)
#define GLW_FLAG_INITIALIZED (1 << 0)
#define GLW_FLAG_ACTIVE      (1 << 1)

#define GLW_IDENTIFIER "DeryabinsGLFramework"

int global_flags = 0;
bool buttons_state[1024];

#define TRY_CALL_VMETHOD(base, method, ...) if(base) (base)->method(__VA_ARGS__);

class window_initializer {
public:
	window_initializer() {
		WNDCLASSA wc;
		memset(&wc, 0, sizeof(wc));
		wc.style = CS_OWNDC;
		wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
		wc.hCursor = LoadCursorW(NULL, MAKEINTRESOURCEW(IDC_ARROW));
		wc.hInstance = GetModuleHandleW(NULL);
		wc.lpszClassName = GLW_IDENTIFIER;
		wc.lpfnWndProc = [](HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) -> LRESULT {
			RECT rect;
			gl_window *p_window = (gl_window *)GetWindowLongPtrA(hwnd, GWL_USERDATA);
			if (msg == WM_CREATE && !p_window)
				return 0;

			iwindowcallbacks *p_callbacks = p_window->get_callbacks();
			switch (msg) {
			case WM_SIZE: {
				GetClientRect(hwnd, &rect);
				TRY_CALL_VMETHOD(p_callbacks, on_resize, rect.right, rect.bottom)
				break;
			}


			default:
				return DefWindowProcA(hwnd, msg, wparam, lparam);
			}
			return 0;
		};

		if (!RegisterClassA(&wc)) {
			MessageBoxA(HWND_DESKTOP, "Failed to initialize window subsystem", "Deryabin's GL framework", MB_OK | MB_ICONERROR);
			exit(-1);
		}
	}

	~window_initializer() {
		UnregisterClassA(GLW_IDENTIFIER, GetModuleHandleW(NULL));
	}
};

#define MAX_ATTRIBS 128

template<int max_count>
class wgl_context_attrib_list
{
	int current;
	int hints[max_count][2];
public:
	wgl_context_attrib_list() : current(0) {}
	~wgl_context_attrib_list() {}

	void push_attrib(int name, int opt) {
		hints[current][0] = name;
		hints[current][1] = opt;
		current++;
	}

	int *get_attribs() { return &hints[0][0]; }
	int  get_num_hints() { return current; }

	void finalize() {
		hints[current][0] = 0;
		hints[current][1] = 0;
	}
};

gl_window::gl_window(long x, long y, long _width, long _height, const char *p_title, const gl_window_hints *p_hints)
{
	HGLRC h_temp_gl_ctx;
	int num_hints = 0;
	wgl_context_attrib_list<MAX_ATTRIBS> attribs;
	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
	PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB;

	xpos = x;
	ypos = y;
	width = _width;
	height = _height;
	h_window = CreateWindowExA(0, GLW_IDENTIFIER, "",
		WS_VISIBLE|WS_OVERLAPPEDWINDOW,
		xpos, ypos, width, height,
		HWND_DESKTOP,
		(HMENU)NULL,
		NULL,
		NULL);

	if (!h_window)
		throw gl_window_exception("failed to create window");

	if (p_title)
		set_title(p_title);

	ShowWindow(h_window, SW_SHOW);

	if (xpos == -1 || ypos == -1)
		move_to_center();

	/* capture context */
	h_devctx = GetDC(h_window);
	if(!h_devctx)
		throw gl_window_exception("GetDC returned NULL");

	/* set pixel format */
	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),  //  size of this pfd  
		1,                     // version number  
		PFD_DRAW_TO_WINDOW |   // support window  
		PFD_SUPPORT_OPENGL |   // support OpenGL  
		PFD_DOUBLEBUFFER,      // double buffered  
		PFD_TYPE_RGBA,         // RGBA type  
		24,                    // 24-bit color depth  
		0, 0, 0, 0, 0, 0,      // color bits ignored  
		0,                     // no alpha buffer  
		0,                     // shift bit ignored  
		0,                     // no accumulation buffer  
		0, 0, 0, 0,            // accum bits ignored  
		32,                    // 32-bit z-buffer      
		0,                     // no stencil buffer  
		0,                     // no auxiliary buffer  
		PFD_MAIN_PLANE,        // main layer  
		0,                     // reserved  
		0, 0, 0                // layer masks ignored  
	};
	int pixel_format = ChoosePixelFormat(h_devctx, &pfd);
	if(!pixel_format)
		throw gl_window_exception("ChoosePixelFormat failed");

	if(!SetPixelFormat(h_devctx, pixel_format, &pfd))
		throw gl_window_exception("SetPixelFormat failed");

	/* create context */
	h_temp_gl_ctx = wglCreateContext(h_devctx);
	if(!h_temp_gl_ctx)
		throw gl_window_exception("wglCreateContext returned NULL");

	if (!wglMakeCurrent(h_devctx, h_temp_gl_ctx))
		throw gl_window_exception("wglMakeCurrent returned FALSE");

	/* check compatibility extensions */
	wglGetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC)wglGetProcAddress("wglGetExtensionsStringARB");
	if (wglGetExtensionsStringARB) {
		const char *p_exts_str = wglGetExtensionsStringARB(h_devctx);

		/* check wglCreateContextAttribsARB support */
		if (strstr(p_exts_str, "WGL_ARB_create_context") || strstr(p_exts_str, "WGL_ARB_create_context_profile")) {
			
			/* set required GL version */
			attribs.push_attrib(WGL_CONTEXT_MAJOR_VERSION_ARB, 3);
			attribs.push_attrib(WGL_CONTEXT_MINOR_VERSION_ARB, 3);

			
			if (strstr(p_exts_str, "ARB_create_context_robustness")) {
				attribs.push_attrib(WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_ROBUST_ACCESS_BIT_ARB);
				attribs.push_attrib(WGL_CONTEXT_RESET_NOTIFICATION_STRATEGY_ARB, WGL_LOSE_CONTEXT_ON_RESET_ARB);
			}

			attribs.finalize();

			wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
			if (!wglCreateContextAttribsARB)
				throw gl_window_exception("wglCreateContextAttribsARB not supported");

			h_glctx = wglCreateContextAttribsARB(h_devctx, NULL, attribs.get_attribs());
			if (!h_glctx)
				throw gl_window_exception("wglCreateContextAttribsARB returned NULL");

			wglMakeCurrent(h_devctx, h_glctx);
			wglDeleteContext(h_temp_gl_ctx);
		}

		return; // OK!
	}
	
	/* old context version */
	h_glctx = h_temp_gl_ctx;
}

gl_window::~gl_window()
{
}

void gl_window::set_title(const char *p_title)
{
	SetWindowTextA(h_window, p_title);
}

void gl_window::get_title(char *p_dst, int maxlen)
{
	GetWindowTextA(h_window, p_dst, maxlen);
}

void gl_window::set_position(long x, long y)
{
}

void gl_window::get_position(long *px, long *py)
{
}

void gl_window::set_size(long width, long height)
{
}

void gl_window::get_size(long *pwidth, long *pheight)
{
}

void gl_window::move_to_center()
{
	int screen_width = GetSystemMetrics(SM_CXSCREEN);
	int screen_height = GetSystemMetrics(SM_CYSCREEN);
	xpos = (screen_width >> 1) - (width >> 1);
	ypos = (screen_height >> 1) - (height >> 1);
	MoveWindow(h_window, xpos, ypos, width, height, FALSE);
}

void gl_window::get_cursor_pos(gl_pt &dst_cur_pos)
{
}

void gl_window::set_cursor_visible(bool b_visible)
{
}

bool gl_window::cursor_is_visible()
{
	return false;
}

bool gl_window::button_is_pressed(int keycode)
{
	return false;
}

bool gl_window::button_is_down(int keycode)
{
	return false;
}

int gl_window::set_mode(int mode)
{
	return 0;
}

int gl_window::get_mode()
{
	return 0;
}

bool gl_window::window_is_should_close()
{
	return flags & GLW_FLAG_ACTIVE;
}

void gl_window::window_close()
{
	if (flags & GLW_FLAG_ACTIVE)
		flags &= ~GLW_FLAG_ACTIVE;
}

void gl_window::poll_events()
{
	MSG msg;
	if (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessageA(&msg);
	}
}

void gl_window::swap_buffers()
{
	SwapBuffers(h_devctx);
}

iwindowcallbacks *gl_window::get_callbacks()
{
	return p_callbacks;
}

void gl_window::set_callbacks(iwindowcallbacks *p_cbs)
{
	p_callbacks = p_cbs;
}

bool gl_window::make_current_context()
{
	return wglMakeCurrent(h_devctx, h_glctx) == TRUE;
}
