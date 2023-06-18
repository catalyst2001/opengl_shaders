/*
 Author: $username$
 Date: $time$
 File: $itemname$
 
*/
#pragma once
#include <stdint.h>

class r_gui_color4
{
public:
	union {
		struct { uint8_t r, g, b, a; };
		uint8_t v[4];
	};

	r_gui_color4() : r(0), g(0), b(0), a(0) {}
	r_gui_color4(uint8_t _r, uint8_t _g, uint8_t _b) : r(_r), g(_g), b(_b), a(255) {}
	r_gui_color4(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a) : r(_r), g(_g), b(_b), a(_a) {}
	r_gui_color4(const uint8_t *p_array) : r(p_array[0]), g(p_array[1]), b(p_array[2]), a(p_array[3]) {}	
	~r_gui_color4() {}

	uint8_t *get_color_ptr() { (uint8_t *)this; }
};