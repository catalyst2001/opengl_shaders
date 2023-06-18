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

typedef uintptr_t rgui_himage;
typedef uintptr_t rgui_hfont;
typedef uintptr_t rgui_hskin;
typedef int rgui_skin_idx;

#define RGUI_INVALID_GROUP_INDEX (-1)

class r_igui_draw
{
public:
	/* IMAGES */
	virtual bool       image_load(rgui_himage &dst_himg, const char *p_path, bool b_flip_vertical) = 0;
	virtual void       image_free(rgui_himage img) = 0;
	virtual void       image_draw(const r_rect &rect, rgui_himage h_img) = 0;
	virtual void       image_draw_uv(const r_rect &rect, const r_uv_rect &uvs, rgui_himage h_img) = 0;

	/* FONTS */
	virtual bool       font_load(rgui_hfont &h_dst_hfont, const char *p_file) = 0;
	virtual void       font_free(rgui_hfont h_font) = 0;
	virtual void       font_get_info(r_font_info &dst_info, rgui_hfont h_font) = 0;
	virtual void       font_draw_glyph(rgui_hfont h_font, int c) = 0;
	virtual void       font_draw_text(rgui_hfont h_font, const char *p_text) = 0;
	virtual void       font_draw_textf(rgui_hfont h_font, const char *p_format, ...) = 0;

	/* SKIN */
	virtual int        skin_load(rgui_hskin &dst_hskin, const char *p_skintext) = 0;
	virtual int        skin_load_from_file(rgui_hskin &dst_hskin, const char *p_hfile) = 0;
	virtual void       skin_free(rgui_hskin h_skin) = 0;
	virtual int        skin_find_group(rgui_skin_idx &dst_idx, const char *p_groupname) = 0;
	virtual r_uv_rect *skin_get_rect_from_idx(rgui_skin_idx group_idx) = 0;
	virtual r_uv_rect *skin_get_uv_rect_from_idx(rgui_skin_idx group_idx) = 0;
};