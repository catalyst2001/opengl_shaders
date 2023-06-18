#pragma once
#include "r_gui_base.h"
#include "r_gui_draw.h"

/*
 Full resizable frame
 No resizable height
*/
class r_gui_frame_FR
{
protected:
	r_uv_rect left_bottom;
	r_uv_rect left;
	r_uv_rect left_top;
	r_uv_rect center_top;
	r_uv_rect right_top;
	r_uv_rect right;
	r_uv_rect right_bottom;
	r_uv_rect center_bottom;

public:
	r_gui_frame_FR();
	~r_gui_frame_FR();

	void draw_frame();
};

/*
 Left Center Right frame
 No resizable width. Height resizeable only
*/
class r_gui_frame_LCR
{
protected:
	r_uv_rect left;
	r_uv_rect center_top;
	r_uv_rect right_top;
	r_uv_rect right;
	r_uv_rect right_bottom;
	r_uv_rect center_bottom;

public:
	r_gui_frame_LCR();
	~r_gui_frame_LCR();

	void draw_frame();
};

/*
 Top Center Bottom frame
 No resizable height. Width resizeable only

  <---> resizable
  +---+
  | T |
  +---+
  |   |
  | C |
  |   |
  +---+
  | B |
  +---+

*/
class r_gui_frame_TCB
{
protected:
	r_uv_rect left;
	r_uv_rect center_top;
	r_uv_rect right_top;
	r_uv_rect right;
	r_uv_rect right_bottom;
	r_uv_rect center_bottom;

public:
	r_gui_frame_TCB();
	~r_gui_frame_TCB();

	void draw_frame();
};