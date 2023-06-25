#pragma once
#include <stdint.h>
#include <vector>
#include "r_gui_rect.h"
#include "r_gui_colors.h"
#include "r_igui_draw.h"

#include "../../common/glad/glad.h"

class r_gui_loader_gl : public r_igui_loader
{
public:
	/* IMAGES */
	virtual bool       image_load(rgui_himage &dst_himg, const char *p_path, bool b_flip_vertical);
	virtual void       image_free(rgui_himage img);

	/* FONTS */
	virtual bool       font_load(rgui_hfont &h_dst_hfont, const char *p_file);
	virtual void       font_free(rgui_hfont h_font);
	virtual void       font_get_info(r_font_info &dst_info, rgui_hfont h_font);

	/* SKIN */
	virtual int        skin_load(rgui_hskin &dst_hskin, const char *p_skintext);
	virtual int        skin_load_from_file(rgui_hskin &dst_hskin, const char *p_hfile);
	virtual void       skin_free(rgui_hskin h_skin);
	virtual int        skin_find_group(rgui_skin_idx &dst_idx, const char *p_groupname);
	virtual r_rect    *skin_get_rect_from_idx(rgui_skin_idx group_idx);
	virtual r_uv_rect *skin_get_uv_rect_from_idx(rgui_skin_idx group_idx);
};

#define TEXTURE_NOT_SET 0
#define DRAWCMD_INITIALIZED (1 << 0)

class r_gui_hw_vertex_buffer_gl
{
	enum GUI_DRAW_BUFFERS {
		DRAW_BUFFER_VBO = 0,
		DRAW_BUFFER_IBO,
		DRAW_BUFFER_COUNT
	};

	GLuint buffers[DRAW_BUFFER_COUNT];

public:
	r_gui_hw_vertex_buffer_gl();

	

};

class r_gui_draw_list
{
protected:


private:
	int flags;
	uint32_t start_vertex;
	r_gui_draw_cmd draw_command;
	GLuint drawing_texture; /* current used for drawcmd texture */
	GLuint current_texture; /* result call of set_texture function */
	size_t total_indices_count;
	std::vector<uint32_t> index_buffer;
	std::vector<r_gui_vertex> vertex_buffer;
	std::vector<r_gui_draw_cmd> commands_buffer;
	

	void   push_current_drawcmd();

//protected:
public:
	r_gui_draw_list(size_t verts_reserve = RGUI_DRAW_RESERVED_VERTICES, size_t cmds_reserve = RGUI_DRAW_RESERVED_COMMANDS);
	~r_gui_draw_list();

	void   set_vertex_buffer_size(size_t size);
	size_t get_vertex_buffer_size();
	void   set_command_buffer_size(size_t size);
	size_t get_command_buffer_size();

	bool   init();
	void   shutdown();

	void   update_fixed(r_gui_vertex *p_verts, uint32_t *p_indices,
		uint32_t verts_offset, uint32_t verts_num,
		uint32_t indices_offset, uint32_t indices_num);

	void   update_fixed_by_info(r_gui_vertex *p_verts, uint32_t *p_indices, drawcmd_info &info) {
		update_fixed(p_verts, p_indices,
			info.get_verts_data_offset(), info.get_num_verts(),
			info.get_indices_data_offset(), info.get_num_indices());
	}

	void   set_texture(GLuint texid);
	void   push_rect(const r_gui_vertex *p_verts);
	void   push_triangle(const r_gui_vertex *p_verts);
	void   query_drawcmd_info(drawcmd_info &info);

	void   clear();
	void   commit();
	void   draw();
};