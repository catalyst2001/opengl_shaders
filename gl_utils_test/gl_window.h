#pragma once
#include <Windows.h>
#include <gl/GL.h>

class gl_pt
{
public:
	long x, y;

	gl_pt() : x(0), y(0) {}
	~gl_pt() {}
};

enum RSYS_STATUS {
	RSYS_STATUS_OK = 0,
	RSYS_STATUS_INVALID_PARAM,

};

/* rsys_window_set_size flags */
#define RSYS_SIZE_FLAG_APPLY_POS    (1 << 0)
#define RSYS_SIZE_FLAG_APPLY_SIZE   (1 << 1)
#define RSYS_SIZE_FLAG_APPLY_CENTER (1 << 2)
#define RSYS_SIZE_FLAG_NO_NOTIFY    (1 << 3)
#define RSYS_SIZE_FLAG_DEFAULT      (RSYS_SIZE_FLAG_APPLY_POS|RSYS_SIZE_FLAG_APPLY_SIZE)

typedef void *rsys_handle;

typedef rsys_handle rsys_hwindow;
typedef rsys_handle rsys_hcursor;
typedef rsys_handle rsys_hicon;
typedef rsys_handle rsys_hctx_attribs_builder;

/* CURSORS */
int rsys_create_cursor(rsys_hcursor *p_dst, int flags, const char *p_path, size_t size);
int rsys_delete_cursor(rsys_hcursor h_cursor);

/* ICON */
int rsys_create_icon(rsys_hicon *p_dst, int flags, const char *p_path, size_t size);
int rsys_delete_icon(rsys_hicon h_icon);

/* CONTEXT PROPERTIES */
int rsys_ctx_attribs_builder_create(rsys_hctx_attribs_builder *p_dst);

#define RSYS_CTX_GL_VERSION_MAJOR 0
#define RSYS_CTX_GL_VERSION_MINOR 1
#define RSYS_CTX_GL_USE_CORE_PROFILE  2
#define RSYS_CTX_USE_ROBUSTNESS_CTX   3
#define RSYS_CTX_ROBUSTNESS_CTX_RESET_STRATEGY 

int rsys_ctx_attribs_builder_set_attrib(rsys_hctx_attribs_builder h_ctx_attrib_builder, int opt, int param);
int rsys_ctx_attribs_builder_delete(rsys_hctx_attribs_builder h_ctx_attrib_builder);

/* 
 window api
*/
int rsys_window_create(rsys_hwindow *p_dst);
int rsys_window_delete(rsys_hwindow h_window);

int rsys_window_set_size(rsys_hwindow h_window, int flags, int x, int y, int width, int height);
int rsys_window_set_title(rsys_hwindow h_window, const char *p_title);
int rsys_window_get_title(char *p_dst, int dstlen, rsys_hwindow h_window);

/* resize callback */
typedef void (*resize_cb)(rsys_hwindow h_window, long width, long height);
int rsys_window_set_size_callback(rsys_hwindow h_window, resize_cb pfn_resize_callback);

/* keyboard callback */
#define RKEY_ESC 27
#define RKEY_RETURN 0
#define RKEY_BACKSPACE 0

#define RCTL_KEY_LSHIFT   (1 << 0)
#define RCTL_KEY_RSHIFT   (1 << 1)
#define RCTL_KEY_LALT     (1 << 2)
#define RCTL_KEY_RALT     (1 << 3)
#define RCTL_KEY_LCTRL    (1 << 4)
#define RCTL_KEY_RCTRL    (1 << 5)

typedef void (*keybd_cb)(int keycode, int state, int ctlkeys);
int rsys_window_set_keybd_callback(rsys_hwindow h_window, keybd_cb pfn_keyboard_callback);

/* mouse callback */
#define RLMOUSE 0
#define RRMOUSE 1
#define RMMOUSE 2

#define RMOUSE_ACTION_MOVE 0
#define RMOUSE_ACTION_CLICK 1
#define RMOUSE_ACTION_

typedef void (*mouse_cb)(long x, long y, int action, int button, int state, int ctlkeys);
int rsys_window_set_mouse_callback(rsys_hwindow h_window, mouse_cb pfn_mouse_callback);

int rsys_window_set_icon(rsys_hwindow h_window, rsys_hicon h_icon);
int rsys_window_get_icon(rsys_hicon *p_dst_icon, rsys_hwindow h_window);

int rsys_window_set_cursor(rsys_hwindow h_window, rsys_hcursor h_src_cursor);
int rsys_window_get_cursor(rsys_hcursor *p_dst, rsys_hwindow h_window);

int rsys_window_initialize(rsys_hwindow h_window, rsys_hctx_attribs_builder h_ctx_attribs_builder);
int rsys_window_deinitialize(rsys_hwindow h_window);

int rsys_window_poll_events(rsys_hwindow h_window);
int rsys_window_swap_buffers(rsys_hwindow h_window);

#if 1

rsys_hwindow h_window;

void keyboard_callback(int keycode, int state, int ctlkeys)
{
}

void window_create_example()
{
	if (rsys_window_create(&h_window) != RSYS_STATUS_OK) {
		printf("Failed to create window!\n");
		return;
	}

	rsys_window_set_size(h_window, RSYS_SIZE_FLAG_APPLY_SIZE|RSYS_SIZE_FLAG_APPLY_CENTER, 0, 0, 800, 600);
	rsys_window_set_title(h_window, "Example window creation");
	rsys_window_set_size_callback(h_window, )
	rsys_window_set_keybd_callback(h_window, keyboard_callback);



}
#endif