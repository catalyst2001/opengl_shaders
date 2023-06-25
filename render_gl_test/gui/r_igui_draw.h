/*
 Author: $username$
 Date: $time$
 File: $itemname$
 
*/
#pragma once
#include <stdint.h>
#include "r_gui_rect.h"

struct r_font_info {
	short size;
	short style;
};

class rgui_handle_internal;

/* COMMON HANDLE CLASS */
class rgui_handle {
	friend class rgui_handle_internal;

	uintptr_t unused;
public:
	rgui_handle() : unused(NULL) {}
	~rgui_handle() {}
};

/* GUI INTERNAL HANDLE */
class rgui_handle_internal {
	uintptr_t handle;
public:
	rgui_handle_internal() : handle(NULL) {}
	rgui_handle_internal(uintptr_t hndl) : handle(hndl) {}
	rgui_handle_internal(rgui_handle hndl) : handle(hndl.unused) {}
	~rgui_handle_internal() {}

	uintptr_t get_handle()                  { return handle; }
	void      set_handle(uintptr_t _handle) { handle = _handle; }
};

/* HANDLE BASE TYPEDEFS */
typedef rgui_handle rgui_himage;
typedef rgui_handle rgui_hfont;
typedef rgui_handle rgui_hskin;
typedef int rgui_skin_idx;

#define RGUI_INVALID_GROUP_INDEX (-1)

#define RGUI_DRAW_RESERVED_VERTICES 8192
#define RGUI_DRAW_RESERVED_COMMANDS 1024

struct r_gui_vertex {
	r_fpt vertex;
	r_uv texcoord;
	r_gui_color4 color;
};

enum RGUI_VERTEX_ATTRIBS {
	RGUI_VERTEX_ATTRIB_VERTEX = 0,
	RGUI_VERTEX_ATTRIB_TEXCOORD,
	RGUI_VERTEX_ATTRIB_COLOR
};

struct r_gui_draw_cmd {
	uint32_t texture;
	uint32_t start_idx;
	uint32_t count_verts;

	void reset() {
		texture = 0;
		start_idx = 0;
		count_verts = 0;
	}
};

/* DRAWCMD INFO */
class drawcmd_info {
	friend class r_gui_draw_list;
	uint32_t verts_start;
	uint32_t num_verts;
	uint32_t indices_start;
	uint32_t num_indices;
public:
	drawcmd_info() : verts_start(0), num_verts(0), indices_start(0), num_indices(0) {}
	~drawcmd_info() {}

	/* SELECTORS */
	uint32_t get_verts_start_index() { return verts_start; }
	uint32_t get_index_start_index() { return indices_start; }
	uint32_t get_num_verts() { return num_verts; }
	uint32_t get_num_indices() { return num_indices; }

	/* HELPERS */
	uint32_t get_verts_data_offset() { return get_verts_start_index() * sizeof(r_gui_vertex); }
	uint32_t get_indices_data_offset() { return get_index_start_index() * sizeof(uint32_t); }
	uint32_t get_verts_data_size() { return get_num_verts() * sizeof(r_gui_vertex); }
	uint32_t get_indices_data_size() { return get_num_indices() * sizeof(uint32_t); }
};

class r_igui_loader
{
public:
	/* IMAGES */
	virtual bool       image_load(rgui_himage &dst_himg, const char *p_path, bool b_flip_vertical) = 0;
	virtual void       image_free(rgui_himage img) = 0;

	/* FONTS */
	virtual bool       font_load(rgui_hfont &h_dst_hfont, const char *p_file) = 0;
	virtual void       font_free(rgui_hfont h_font) = 0;
	virtual void       font_get_info(r_font_info &dst_info, rgui_hfont h_font) = 0;

	/* SKIN */
	virtual int        skin_load(rgui_hskin &dst_hskin, const char *p_skintext) = 0;
	virtual int        skin_load_from_file(rgui_hskin &dst_hskin, const char *p_hfile) = 0;
	virtual void       skin_free(rgui_hskin h_skin) = 0;
	virtual int        skin_find_group(rgui_skin_idx &dst_idx, const char *p_groupname) = 0;
	virtual r_rect    *skin_get_rect_from_idx(rgui_skin_idx group_idx) = 0;
	virtual r_uv_rect *skin_get_uv_rect_from_idx(rgui_skin_idx group_idx) = 0;
};

class r_igui_drawer
{
public:
	virtual rgui_himage set_image(rgui_himage h_img) = 0;

	virtual void       push_glyph(rgui_hfont h_font, int c) = 0;
	virtual void       push_text(rgui_hfont h_font, const char *p_text) = 0;
	virtual void       push_textf(rgui_hfont h_font, const char *p_format, ...) = 0;

	virtual void       push_vertex(r_gui_vertex *p_vertex) = 0;
	virtual void       push_triangle(r_gui_vertex *p_verts) = 0;
	virtual void       push_rect(r_gui_vertex *p_verts) = 0;
};