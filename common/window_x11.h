#pragma once
#include "window.h"

typedef struct window_x11_s {
	int c;
} window_x11_t;

r_window_t r_window_create(const r_window_init_data_t *p_cdata);
void r_window_destroy_window(r_window_t h_window);
bool r_window_change_mode(r_window_t h_window, const char *p_title, WINDOW_CHANGE_MODE_FLAGS flags, WINDOW_MODES mode, int width, int height);
bool r_window_destroy(r_window_t h_window);
void r_window_swap_buffers(r_window_t h_window);