/*
 Author: $username$
 Date: $time$
 File: r_gui_draw_gl.h
 
*/
#pragma once
#include "r_gui_draw.h"

class r_draw_gl : public r_igui_draw
{
public:
	/* IMAGES */
	virtual bool       image_load(rgui_himage &dst_himg, const char *p_path, bool b_flip_vertical);
	virtual void       image_free(rgui_himage img);
	virtual void       image_draw(const r_rect &rect, rgui_himage h_img);
	virtual void       image_draw_uv(const r_rect &rect, const r_uv_rect &uvs, rgui_himage h_img);

	/* FONTS */
	virtual bool       font_load(rgui_hfont &h_dst_hfont, const char *p_file);
	virtual void       font_free(rgui_hfont h_font);
	virtual void       font_get_info(r_font_info &dst_info, rgui_hfont h_font);
	virtual void       font_draw_glyph(rgui_hfont h_font, int c);
	virtual void       font_draw_text(rgui_hfont h_font, const char *p_text);
	virtual void       font_draw_textf(rgui_hfont h_font, const char *p_format, ...);

	/* SKIN */
	virtual int        skin_load(rgui_hskin &dst_hskin, const char *p_skintext);
	virtual int        skin_load_from_file(rgui_hskin &dst_hskin, const char *p_hfile);
	virtual void       skin_free(rgui_hskin h_skin);
	virtual int        skin_find_group(rgui_skin_idx &dst_idx, const char *p_groupname);
	virtual r_uv_rect *skin_get_rect_from_idx(rgui_skin_idx group_idx);
	virtual r_uv_rect *skin_get_uv_rect_from_idx(rgui_skin_idx group_idx);
};