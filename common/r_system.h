#pragma once
#include <stdbool.h>

typedef struct r_size_s {
	int width;
	int height;
} r_size_t;

typedef struct r_point_s {
	int x, y;
} r_point_t;

void rsys_get_screen_size(r_size_t *p_dst);
void rsys_get_cursor_pos(r_point_t *p_dst);
void rsys_set_cursor_pos(const r_point_t *p_src);

/* ENUM VIDEO MODES */
typedef struct r_videomode_s {
	short unused;
} r_videomode_t;

bool rsys_enum_videomodes(r_videomode_t *p_vvideomodes, int count, int *p_dst_count);

/* ENUM MONITORS */
typedef struct r_monitor_info_s {
	short unused;
} r_monitor_info_t;

bool rsys_enum_monitors(r_monitor_info_t *p_vmonitors, int count, int *p_dst_count);

/* ENUM VIDEO ADAPTERS */
typedef struct r_adapter_info_s {
	short unused;
} r_adapter_info_t;

bool rsys_enum_adapters(r_adapter_info_t *p_vadapters, int count, int *p_dst_count);