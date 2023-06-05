/*
 Author: Deryabin K.
 Date: 06.06.2023
 File: window.h
 
*/
#pragma once

#define RW_DEFAULT_TITLE "React Game Engine Scene Window"
#define RW_DEFAULT_WCLASS "RGE060623"

typedef struct r_window_s {
	void *ptr;
} r_window_t;

typedef enum WINDOW_CHANGE_MODE_FLAGS_ {
	CHANGE_MODE_FLAG_FULLSCREEN = (1 << 0),
	CHANGE_MODE_FLAG_SIZE = (1 << 1),
	CHANGE_MODE_FLAG_WINDOWTITLE = (1 << 2),
	CHANGE_MODE_ALL = (CHANGE_MODE_FLAG_FULLSCREEN|CHANGE_MODE_FLAG_SIZE)
} WINDOW_CHANGE_MODE_FLAGS;

typedef enum WINDOW_MODES_ {
	WINDOW_MODE_WINDOWED = 0,
	WINDOW_MODE_RECT,
	WINDOW_MODE_FULLSCREEN
} WINDOW_MODES;

/* default window parameters */
#define RW_DEFAULT_WIDTH             (640)
#define RW_DEFAULT_HEIGHT            (480)
#define RW_DEFAULT_COLOR_BUFFER_BITS (24)
#define RW_DEFAULT_DEPTH_BUFFER_BITS (32)
#define RW_DEFAULT_MODE              (WINDOW_MODE_FULLSCREEN)

/* GL context requires */
#define RW_MIN_GL_CONTEXT_VER_MAJOR  (2)
#define RW_MIN_GL_CONTEXT_VER_MINOR  (0)

/* window init data structure */
typedef struct r_window_init_data_s {
	const char *p_wclass;
	const char *p_title;
	long x;
	long y;
	long width;
	long height;
	long color_bits;
	long depth_bits;
	WINDOW_MODES wmode;
} r_window_init_data_t;