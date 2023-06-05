#pragma once
#include "window.h"

typedef struct window_x11_s {
	int c;
} window_x11_t;

r_window_t r_window_create(const r_window_init_data_t *p_cdata);
r_window_pool_events(r_window_t h_window);
r_window_destroy_window(r_window_t h_window);
r_window_change_mode(r_window_t h_window, );
r_window_destroy(r_window_t h_window);
r_window_swap_buffers(r_window_t h_window);