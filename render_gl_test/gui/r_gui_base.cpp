#include "r_gui_base.h"

bool r_gui_base::_find_child(size_t *p_dst_idx, r_gui_base *p_child_addr)
{
	for (size_t i = 0; i < get_num_childs(); i++) {
		if (childs[i] == p_child_addr) {
			if (p_dst_idx)
				*p_dst_idx = i;

			return true;
		}
	}
	return false;
}

bool r_gui_base::_is_valid_index(size_t _idx)
{
	return _idx < get_num_childs();
}

bool r_gui_base::_remove_child(size_t _idx)
{
	if (_is_valid_index(_idx) /*&& childs.size()*/) {
		childs.erase(childs.begin() + _idx);
		return true;
	}
	return false;
}

r_gui_base::r_gui_base(r_gui_base *_p_parent, unsigned int _level, const char *p_elem_name, int _flags, int x, int y, int width, int height) : flags(_flags), rect(r_rect(x, y, width, height)), level((!_p_parent) ? 0 : (_level + 1)), p_name(p_elem_name), p_parent(_p_parent)
{
}

r_gui_base::~r_gui_base()
{
	for (size_t i = 0; i < childs.size(); i++)
		if(childs[i])
			delete childs[i];
}

void r_gui_base::on_key_down(int keycode, int state)
{
}

void r_gui_base::on_mosue_key_down(int x, int y, int key, int xkey, int state)
{
}

void r_gui_base::on_mouse_move(int x, int y)
{
}

void r_gui_base::on_char_input(int c)
{
}

void r_gui_base::on_notify_message(int message)
{
}

void r_gui_base::on_parent_resize(int width, int height)
{
}

const char *r_gui_base::get_name()
{
	return p_name;
}

void r_gui_base::get_rect(r_rect &dst_rect)
{
	dst_rect = rect;
}

size_t r_gui_base::get_num_childs()
{
	return childs.size();
}

r_igui_base_methods *r_gui_base::get_child(size_t _child_idx)
{
	if (_child_idx >= get_num_childs())
		return nullptr;

	return childs[_child_idx];
}

bool r_gui_base::add_child(r_igui_base_methods *p_child)
{
	if (this == p_child)
		return false; // prevent stack overflow in infinity loop r_igui_base_callbacks method calls

	if (_find_child(NULL, (r_gui_base *)p_child))
		return false;

	childs.push_back((r_gui_base *)p_child);
	return true;
}

bool r_gui_base::rem_child(size_t _child_idx)
{
	return _remove_child(_child_idx);
}

unsigned int r_gui_base::get_level()
{
	return level;
}
