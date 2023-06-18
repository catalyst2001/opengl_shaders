/*
 React Engine Retained UI System Soruces
 Version: 1.1
 Purpose: Base class for all user interface elements
 
 Author: Deryabin K.
 Created: 11.06.2023
 Modified: 11.06.2023
*/
#pragma once
#include <vector>
#include "r_gui_rect.h"

#define RGUI_FLAG_NONE         (0) // none flags
#define RGUI_FLAG_VISIBLE      (1 << 0) // will be drawn
#define RGUI_FLAG_INPUT        (1 << 1) // will accept input messages
#define RGUI_FLAG_CHILDSNOTIFY (1 << 2) // will notify children about the occurrence of an event
#define RGUI_FLAG_RESIZEABLE   (1 << 3) // size can be customized
#define RGUI_FLAG_HAVECHILDS   (1 << 4) // may have children
#define RGUI_FLAG_FOCUS        (1 << 5) // (NO SET) must be set by root UI dispatcher if this element is using currently
#define RGUI_FLAG_ALL          (RGUI_FLAG_VISIBLE|RGUI_FLAG_INPUT|RGUI_FLAG_CHILDSNOTIFY|RGUI_FLAG_RESIZEABLE|RGUI_FLAG_HAVECHILDS)

class r_gui_base;

class r_igui_base_callbacks
{
protected:
	virtual void                 on_key_down(int keycode, int state) = 0;
	virtual void                 on_mosue_key_down(int x, int y, int key, int xkey, int state) = 0;
	virtual void                 on_mouse_move(int x, int y) = 0;
	virtual void                 on_char_input(int c) = 0;
	virtual void                 on_notify_message(int message) = 0;
	virtual void                 on_parent_resize(int width, int height) = 0;
};

class r_igui_base_methods
{
public:
	virtual const char          *get_name() = 0;
	virtual void                 get_rect(r_rect &dst_rect) = 0;
	virtual size_t               get_num_childs() = 0;
	virtual r_igui_base_methods *get_child(size_t _child_idx) = 0;
	virtual bool                 add_child(r_igui_base_methods *p_child) = 0;
	virtual bool                 rem_child(size_t _child_idx) = 0;
	virtual unsigned int         get_level() = 0;
};

class r_gui_base : r_igui_base_callbacks, r_igui_base_methods
{
protected:
	int flags;
	r_rect rect;
	unsigned int level;
	const char *p_name;
	r_gui_base *p_parent;
	std::vector<r_gui_base *> childs;

	bool _find_child(size_t *p_dst_idx, r_gui_base *p_child_addr);
	bool _is_valid_index(size_t _idx);
	bool _remove_child(size_t _idx);

	r_gui_base(r_gui_base *_p_parent, unsigned int _level, const char *p_elem_name, int flags, int x, int y, int width, int height);
	~r_gui_base();

	/* r_igui_base_callbacks */
	virtual void                 on_key_down(int keycode, int state);
	virtual void                 on_mosue_key_down(int x, int y, int key, int xkey, int state);
	virtual void                 on_mouse_move(int x, int y);
	virtual void                 on_char_input(int c);
	virtual void                 on_notify_message(int message);
	virtual void                 on_parent_resize(int width, int height);

	/* r_igui_base_methods */
	virtual const char          *get_name();
	virtual void                 get_rect(r_rect &dst_rect);
	virtual size_t               get_num_childs();
	virtual r_igui_base_methods *get_child(size_t _child_idx);
	virtual bool                 add_child(r_igui_base_methods *p_child);
	virtual bool                 rem_child(size_t _child_idx);
	virtual unsigned int         get_level();
};

