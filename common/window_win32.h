#pragma once
#include <windows.h>
#include "window.h"

#define WGL_CONTEXT_DEBUG_BIT_ARB      0x00000001
#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB 0x00000002
#define WGL_CONTEXT_MAJOR_VERSION_ARB  0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB  0x2092
#define WGL_CONTEXT_LAYER_PLANE_ARB    0x2093
#define WGL_CONTEXT_FLAGS_ARB          0x2094
#define ERROR_INVALID_VERSION_ARB      0x2095

#define WGL_CONTEXT_PROFILE_MASK_ARB   0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002
#define ERROR_INVALID_PROFILE_ARB      0x2096

typedef HGLRC (WINAPI *PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC hDC, HGLRC hShareContext, const int *attribList);

typedef struct window_win32_s {
	const char *p_classname;
	HWND h_window;
	HDC h_dev_ctx;
	HGLRC h_gl_ctx;
} window_win32_t;

int r_window_create(r_window_t *p_dst_window, const r_window_init_data_t *p_cdata);
void r_window_destroy_window(r_window_t h_window);
bool r_window_change_mode(r_window_t h_window, const char *p_title, WINDOW_CHANGE_MODE_FLAGS flags, WINDOW_MODES mode, int width, int height);
bool r_window_destroy(r_window_t h_window);
void r_window_swap_buffers(r_window_t h_window);
void r_window_get_info(r_window_info_t *p_dst, r_window_t h_window);
