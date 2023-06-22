#include "r_gui_draw.h"

/*
 Author: $username$
 Date: $time$
 File: $itemname$
 Purpose: GL draw implements
*/
#include "r_gl_errors.h"
#include "r_gui_image_utils.h"

#ifdef _DEBUG
#define RGUI_DRAW_DBGPRINT(a, ...) printf(__FUNCSIG__": " a "\n", __VA_ARGS__)
#else
#define RGUI_DRAW_DBGPRINT(a, ...)
#endif

/*
	GLuint new_vbo;
	if (GLAD_GL_VERSION_3_1) {
		glGenBuffers(1, &new_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, new_vbo);
		glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)vertex_buffer.size(), NULL, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_COPY_READ_BUFFER, gui_vbo);
		glBindBuffer(GL_COPY_WRITE_BUFFER, new_vbo);
		glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, vbo_size);

		glDeleteBuffers(1, &gui_vbo);
		gui_vbo = new_vbo;
	}
	else
	{
		// alloc new buffer
		glGenBuffers(1, &new_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, new_vbo);
		glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)vertex_buffer.size(), NULL, GL_DYNAMIC_DRAW);

		// copy from old buffer
		glBindBuffer(GL_COPY_READ_BUFFER, gui_vbo);
		glBindBuffer(GL_COPY_WRITE_BUFFER, new_vbo);
		GLvoid* oldData = glMapBuffer(GL_COPY_READ_BUFFER, GL_READ_ONLY);
		glBufferSubData(GL_COPY_WRITE_BUFFER, 0, vbo_size, oldData);
		glUnmapBuffer(GL_COPY_READ_BUFFER);
	}
*/

bool r_gui_draw::image_load(rgui_himage &dst_himg, const char *p_path, bool b_flip_vertical)
{
	r_image image(p_path, b_flip_vertical);
	if (image.get_status() != r_image::IMGLDR_STATUS_OK)
		return false;

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.get_width(), image.get_height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.get_image_data());
	glBindTexture(GL_TEXTURE_2D, 0);
	return true;
}

void r_gui_draw::image_free(rgui_himage img)
{
	GLuint texture = (GLuint)img;
	if (glIsTexture(texture))
		glDeleteTextures(1, &texture);
}

void r_gui_draw::image_draw(const r_rect &rect, rgui_himage h_img)
{
	




}

void r_gui_draw::image_draw_uv(const r_rect &rect, const r_uv_rect &uvs, rgui_himage h_img)
{
}

bool r_gui_draw::font_load(rgui_hfont &h_dst_hfont, const char *p_file)
{
	return false;
}

void r_gui_draw::font_free(rgui_hfont h_font)
{
}

void r_gui_draw::font_get_info(r_font_info &dst_info, rgui_hfont h_font)
{
}

void r_gui_draw::font_draw_glyph(rgui_hfont h_font, int c)
{
}

void r_gui_draw::font_draw_text(rgui_hfont h_font, const char *p_text)
{
}

void r_gui_draw::font_draw_textf(rgui_hfont h_font, const char *p_format, ...)
{
}

int r_gui_draw::skin_load(rgui_hskin &dst_hskin, const char *p_skintext)
{
	return 0;
}

int r_gui_draw::skin_load_from_file(rgui_hskin &dst_hskin, const char *p_hfile)
{
	return 0;
}

void r_gui_draw::skin_free(rgui_hskin h_skin)
{
}

int r_gui_draw::skin_find_group(rgui_skin_idx &dst_idx, const char *p_groupname)
{
	return 0;
}

r_uv_rect *r_gui_draw::skin_get_rect_from_idx(rgui_skin_idx group_idx)
{
	return nullptr;
}

r_uv_rect *r_gui_draw::skin_get_uv_rect_from_idx(rgui_skin_idx group_idx)
{
	return nullptr;
}

void r_gui_draw_list::push_current_drawcmd()
{
	draw_command.texture = drawing_texture;
	draw_command.count_verts = (GLuint)index_buffer.size() - draw_command.start_idx;
	if(draw_command.count_verts)
		commands_buffer.push_back(draw_command);
}

r_gui_draw_list::r_gui_draw_list(size_t verts_reserve, size_t cmds_reserve)
{
	flags = 0;
	if(verts_reserve)
		vertex_buffer.reserve(verts_reserve);

	if(cmds_reserve)
		commands_buffer.reserve(cmds_reserve);

	start_vertex = 0;
	draw_command.reset();
}

r_gui_draw_list::~r_gui_draw_list()
{
}

void r_gui_draw_list::set_vertex_buffer_size(size_t size)
{
	vertex_buffer.resize(size);
}

size_t r_gui_draw_list::get_vertex_buffer_size()
{
	return vertex_buffer.size();
}

void r_gui_draw_list::set_command_buffer_size(size_t size)
{
	commands_buffer.resize(size);
}

size_t r_gui_draw_list::get_command_buffer_size()
{
	return commands_buffer.size();
}

bool r_gui_draw_list::init()
{
	GL_CALL(glGenBuffers(DRAW_LIST_BUFFER_COUNT, buffers));
	glBindBuffer(GL_ARRAY_BUFFER, buffers[DRAW_LIST_VBO]);
	GL_CALL(glVertexAttribPointer(RGUI_VERTEX_ATTRIB_VERTEX, 2, GL_FLOAT, GL_FALSE, sizeof(r_gui_vertex), (const char *)offsetof(r_gui_vertex, vertex)));
	GL_CALL(glEnableVertexAttribArray(RGUI_VERTEX_ATTRIB_VERTEX));
	GL_CALL(glVertexAttribPointer(RGUI_VERTEX_ATTRIB_TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(r_gui_vertex), (const char *)offsetof(r_gui_vertex, texcoord)));
	GL_CALL(glEnableVertexAttribArray(RGUI_VERTEX_ATTRIB_TEXCOORD));
	GL_CALL(glVertexAttribPointer(RGUI_VERTEX_ATTRIB_COLOR, 4, GL_FLOAT, GL_FALSE, sizeof(r_gui_vertex), (const char *)offsetof(r_gui_vertex, color)));
	GL_CALL(glEnableVertexAttribArray(RGUI_VERTEX_ATTRIB_COLOR));
	return true;
}

void r_gui_draw_list::shutdown()
{
	glDeleteBuffers(DRAW_LIST_BUFFER_COUNT, buffers);
}

void r_gui_draw_list::update_fixed(r_gui_vertex *p_verts, uint32_t *p_indices, uint32_t verts_offset, uint32_t verts_num, uint32_t indices_offset, uint32_t indices_num)
{
}

void r_gui_draw_list::set_texture(GLuint texid)
{
	current_texture = texid;

	// set first texture
	if (drawing_texture == TEXTURE_NOT_SET) {
		drawing_texture = current_texture;
		return;
	}

	// changing texture, save old draw command and init new
	if (drawing_texture != current_texture) {
		push_current_drawcmd(); // push previous primitive draw command
<<<<<<< HEAD
		draw_command.reset(); // reset values in draw command
		draw_command.start_idx = (GLuint)index_buffer.size(); // save start index in index buffer
		start_vertex = (uint32_t)vertex_buffer.size(); // save start vertex for new draw command
		drawing_texture = current_texture; // replace drawing texture to current
=======

		draw_command.reset();
		draw_command.start_idx = (GLuint)index_buffer.size();
>>>>>>> parent of 0ca0d1f (indices error fixed)
	}
}

void r_gui_draw_list::push_rect(const r_gui_vertex *p_verts)
{
	uint32_t index = (uint32_t)index_buffer.size();
	vertex_buffer.push_back(p_verts[0]);
	vertex_buffer.push_back(p_verts[1]);
	vertex_buffer.push_back(p_verts[2]);
	vertex_buffer.push_back(p_verts[3]);

	index_buffer.push_back(index + 0);
	index_buffer.push_back(index + 1);
	index_buffer.push_back(index + 2);

	index_buffer.push_back(index + 2);
	index_buffer.push_back(index + 3);
	index_buffer.push_back(index + 0);
	RGUI_DRAW_DBGPRINT("%zd verts  %zd indices", vertex_buffer.size(), index_buffer.size());
}

void r_gui_draw_list::push_triangle(const r_gui_vertex *p_verts)
{
	uint32_t index = (uint32_t)index_buffer.size();
	vertex_buffer.push_back(p_verts[0]);
	vertex_buffer.push_back(p_verts[1]);
	vertex_buffer.push_back(p_verts[2]);

	index_buffer.push_back(index + 0);
	index_buffer.push_back(index + 1);
	index_buffer.push_back(index + 2);
	RGUI_DRAW_DBGPRINT("%zd verts  %zd indices", vertex_buffer.size(), index_buffer.size());
}

void r_gui_draw_list::query_drawcmd_info(drawcmd_info &info)
{
	info.verts_start = start_vertex;
	info.num_verts = (uint32_t)vertex_buffer.size() - start_vertex;
	info.indices_start = draw_command.start_idx;
	info.num_indices = draw_command.count_verts;
}

void r_gui_draw_list::clear()
{
	vertex_buffer.clear();
	index_buffer.clear();
	commands_buffer.clear();
}

//#define GUI_DEBUG

void r_gui_draw_list::commit()
{
	GLint vbo_size, ibo_size;
	GLint previous_vbo, previous_ibo;

#ifdef GUI_DEBUG
	size_t i;

	/* print verts */
	for (i = 0; i < vertex_buffer.size(); i++) {
		r_gui_vertex &vert = vertex_buffer[i];
		RGUI_DRAW_DBGPRINT("%d  vert( %f %f ) uv( %f %f ) color( %f %f %f %f )", i,
			vert.vertex.x, vert.vertex.y,
			vert.texcoord.u, vert.texcoord.v,
			vert.color.r, vert.color.g, vert.color.b, vert.color.a);
	}

	/* print indices */
	for (i = 0; i < index_buffer.size(); i++)
		RGUI_DRAW_DBGPRINT("%d  index(%d)", index_buffer[i]);

	/* triangles */
<<<<<<< HEAD
	for (i = 0; i < index_buffer.size(); i += 3) {
		uint32_t i0 = index_buffer[i + 0];
		uint32_t i1 = index_buffer[i + 1];
		uint32_t i2 = index_buffer[i + 2];
		r_fpt &v0 = vertex_buffer[i0].vertex;
		r_fpt &v1 = vertex_buffer[i1].vertex;
		r_fpt &v2 = vertex_buffer[i2].vertex;
		RGUI_DRAW_DBGPRINT("%d  triangle( {%f %f}, {%f %f}, {%f, %f} )", v0.x, v0.y, v1.x, v1.y, v2.x, v2.y);
=======
	//for (i = 0; i < index_buffer.size(); i += 3) {
	//	uint32_t i0 = index_buffer[i + 0];
	//	uint32_t i1 = index_buffer[i + 1];
	//	uint32_t i2 = index_buffer[i + 2];
	//	r_fpt &v0 = vertex_buffer[i0].vertex;
	//	r_fpt &v1 = vertex_buffer[i1].vertex;
	//	r_fpt &v2 = vertex_buffer[i2].vertex;
	//	RGUI_DRAW_DBGPRINT("%d  triangle( {%f %f}, {%f %f}, {%f, %f} )", v0.x, v0.y, v1.x, v1.y, v2.x, v2.y);
	//}

	for (i = 0; i < index_buffer.size(); i++) {
		r_fpt &v0 = vertex_buffer[index_buffer[i]].vertex;
		RGUI_DRAW_DBGPRINT("%d  vt(%f %f)", v0.x, v0.y);
>>>>>>> parent of 0ca0d1f (indices error fixed)
	}

	//for (i = 0; i < index_buffer.size(); i++) {
	//	r_fpt &v0 = vertex_buffer[index_buffer[i]].vertex;
	//	RGUI_DRAW_DBGPRINT("%d  vt(%f %f)", i, v0.x, v0.y);
	//}

#endif

	/* store old bindings */
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &previous_vbo);
	glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &previous_ibo);

	push_current_drawcmd(); // save unfinished end drawcmd

	/* bind GUI VBO/IBO */
	vbo_size = 0;
	ibo_size = 0;
	glBindBuffer(GL_ARRAY_BUFFER, buffers[DRAW_LIST_VBO]);
	glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &vbo_size);
	vbo_size /= sizeof(r_gui_vertex);
	if (vbo_size < (GLint)vertex_buffer.size()) {
		GL_CALL(glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(vertex_buffer.size() * sizeof(r_gui_vertex)), vertex_buffer.data(), GL_DYNAMIC_DRAW));
		RGUI_DRAW_DBGPRINT("glBufferData(GL_ARRAY_BUFFER) allocated %zd bytes", vertex_buffer.size() * sizeof(r_gui_vertex));
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[DRAW_LIST_IBO]);
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &ibo_size);
	ibo_size /= sizeof(r_gui_vertex);
	if (ibo_size < (GLint)index_buffer.size()) {
		GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)(index_buffer.size() * sizeof(uint32_t)), index_buffer.data(), GL_DYNAMIC_DRAW));
		RGUI_DRAW_DBGPRINT("glBufferData(GL_ELEMENT_ARRAY_BUFFER) allocated %zd bytes", index_buffer.size() * sizeof(uint32_t));
		total_indices_count = index_buffer.size();
	}

	/* restore old bindings */
	glBindBuffer(GL_ARRAY_BUFFER, previous_vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, previous_ibo);
}

void r_gui_draw_list::draw()
{
	GLuint previous_texture = 0;
	glBindBuffer(GL_ARRAY_BUFFER, buffers[DRAW_LIST_VBO]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[DRAW_LIST_IBO]);
	for (size_t i = 0; i < commands_buffer.size(); i++) {
		r_gui_draw_cmd &drawcmd = commands_buffer[i];

		/* bind used texture */
		if (previous_texture != drawcmd.texture) {
			glBindTexture(GL_TEXTURE_2D, drawcmd.texture);
			previous_texture = drawcmd.texture;
		}
		glDrawElements(GL_TRIANGLES, drawcmd.count_verts, GL_UNSIGNED_INT, (void *)(drawcmd.start_idx * sizeof(GLuint)));
	}
}
