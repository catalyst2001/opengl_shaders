/*
 Author: Deryabin K.
 Date: 06.06.2023
 File: window.h
 
*/
#pragma once
#include <stdbool.h>
#include "r_status.h"

#define RW_DEFAULT_TITLE "React Game Engine Scene Window"
#define RW_DEFAULT_WCLASS "RGE060623"

typedef struct r_window_s {
	void *ptr;
} r_window_t;

#define TO_WINDOW_HANDLE(ptr) (*((r_window_t *)ptr))

typedef enum WINDOW_CHANGE_MODE_FLAGS_ {
	CHANGE_MODE_FLAG_FULLSCREEN = (1 << 0),
	CHANGE_MODE_FLAG_SIZE = (1 << 1),
	CHANGE_MODE_FLAG_WINDOWTITLE = (1 << 2),
	CHANGE_MODE_ALL = (CHANGE_MODE_FLAG_FULLSCREEN|CHANGE_MODE_FLAG_SIZE|CHANGE_MODE_FLAG_WINDOWTITLE)
} WINDOW_CHANGE_MODE_FLAGS;

typedef enum WINDOW_MODES_ {
	WINDOW_MODE_WINDOWED = 0,
	WINDOW_MODE_RECT,
	WINDOW_MODE_FULLSCREEN,

	WINDOW_MODE_NONE
} WINDOW_MODES;

/* default window parameters */
#define RW_DEFAULT_WIDTH             (640)
#define RW_DEFAULT_HEIGHT            (480)
#define RW_DEFAULT_COLOR_BUFFER_BITS (32)
#define RW_DEFAULT_DEPTH_BUFFER_BITS (24)
#define RW_DEFAULT_MODE              (WINDOW_MODE_FULLSCREEN)

/* GL context requires */
#define RW_MIN_GL_CONTEXT_VER_MAJOR  (2)
#define RW_MIN_GL_CONTEXT_VER_MINOR  (0)

/* window init data structure */
typedef struct r_window_init_data_s {
	const char *p_wclass;
	const char *p_title;
	long width;
	long height;
	long color_bits;
	long depth_bits;
	WINDOW_MODES wmode;
} r_window_init_data_t;

typedef struct r_window_info_s {
	char title[64];
	int colorbits;
	int depthbits;
	int posx;
	int posy;
	int width;
	int height;
} r_window_info_t;

enum R_KEYSTATE {
	R_KEYSTATE_UP = 0,
	R_KEYSTATE_DOWN,
};

enum R_MOUSEBUTTON {
	R_MOUSE_LEFT_BUTTON = 0,
	R_MOUSE_RIGHT_BUTTON,
	R_MOUSE_MIDDLE_BUTTON,
	R_MOUSE_XBUTTON
};

typedef void (*key_event_pfn)(int keycode, int keystate);
typedef void (*wheel_event_pfn)(int wheel_delta);
typedef void (*mouse_event_pfn)(int button, int xbutton, int keystate);

int  r_window_create(r_window_t *p_dst_window, const r_window_init_data_t *p_cdata);
bool r_window_change_mode(r_window_t h_window, const char *p_title, WINDOW_CHANGE_MODE_FLAGS flags, WINDOW_MODES mode, int width, int height);
bool r_window_destroy(r_window_t h_window);
void r_window_swap_buffers(r_window_t h_window);
void r_window_get_info(r_window_info_t *p_dst, r_window_t h_window);
bool r_window_keybd_state(r_window_t h_window, int keycode);
bool r_window_mouse_keys_state();