#pragma once
#include <stdint.h>
#include <vector>
#include "r_gui_rect.h"
#include "r_gui_colors.h"
#include "r_igui_draw.h"

#include "../../common/glad/glad.h"

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
	GLuint texture;
	GLuint start_idx;
	GLuint count_verts;

	void reset() {
		texture = 0;
		start_idx = 0;
		count_verts = 0;
	}
};

class r_gui_draw
{
public:
	/* IMAGES */
	static bool       image_load(rgui_himage &dst_himg, const char *p_path, bool b_flip_vertical);
	static void       image_free(rgui_himage img);
	static void       image_draw(const r_rect &rect, rgui_himage h_img);
	static void       image_draw_uv(const r_rect &rect, const r_uv_rect &uvs, rgui_himage h_img);

	/* FONTS */
	static bool       font_load(rgui_hfont &h_dst_hfont, const char *p_file);
	static void       font_free(rgui_hfont h_font);
	static void       font_get_info(r_font_info &dst_info, rgui_hfont h_font);
	static void       font_draw_glyph(rgui_hfont h_font, int c);
	static void       font_draw_text(rgui_hfont h_font, const char *p_text);
	static void       font_draw_textf(rgui_hfont h_font, const char *p_format, ...);

	/* SKIN */
	static int        skin_load(rgui_hskin &dst_hskin, const char *p_skintext);
	static int        skin_load_from_file(rgui_hskin &dst_hskin, const char *p_hfile);
	static void       skin_free(rgui_hskin h_skin);
	static int        skin_find_group(rgui_skin_idx &dst_idx, const char *p_groupname);
	static r_uv_rect *skin_get_rect_from_idx(rgui_skin_idx group_idx);
	static r_uv_rect *skin_get_uv_rect_from_idx(rgui_skin_idx group_idx);
};

#define TEXTURE_NOT_SET 0
#define DRAWCMD_INITIALIZED (1 << 0)

class r_gui_draw_list
{
protected:
	enum GUI_DRAW_LIST_BUFFERS {
		DRAW_LIST_VBO = 0,
		DRAW_LIST_IBO,
		DRAW_LIST_BUFFER_COUNT
	};

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
	GLuint buffers[DRAW_LIST_BUFFER_COUNT];

	void   push_current_drawcmd();

//protected:
public:

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
		uint32_t get_verts_start_index() { return verts_start;  }
		uint32_t get_index_start_index() { return indices_start; }
		uint32_t get_num_verts() { return num_verts; }
		uint32_t get_num_indices() { return num_indices; }

		/* HELPERS */
		uint32_t get_verts_data_offset() { return get_verts_start_index() * sizeof(r_gui_vertex); }
		uint32_t get_indices_data_offset() { return get_index_start_index() * sizeof(GLuint); }
		uint32_t get_verts_data_size() { return get_num_verts() * sizeof(r_gui_vertex); }
		uint32_t get_indices_data_size() { return get_num_indices() * sizeof(GLuint); }
	};

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