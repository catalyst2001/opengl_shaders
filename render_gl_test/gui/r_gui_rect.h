/*
 Author: $username$
 Date: $time$
 File: $itemname$
 
*/
#pragma once

class r_rect;

class r_pt
{
public:
	int x, y;

	r_pt() : x(0), y(0) {}
	r_pt(int _x, int _y) : x(_x), y(_y) {}
	r_pt(float _x, float _y) : x((int)_x), y((int)_y) {}
	~r_pt() {}

};

class r_fpt
{
public:
	float x, y;

	r_fpt() : x(0.f), y(0.f) {}
	r_fpt(float _x, float _y) : x(_x), y(_y) {}
	r_fpt(int _x, int _y) : x((float)_x), y((float)_y) {}
	r_fpt(r_pt ipt) : x((float)ipt.x), y((float)ipt.y) {}
	~r_fpt() {}

};

struct r_uv
{
	float u, v;

	r_uv() : u(0.f), v(0.f) {}
	r_uv(float U, float V) : u(U), v(V) {}
	~r_uv() {}
};

class r_uv_rect
{
	friend class r_rect;

	union {
		struct {
			r_uv p0;
			r_uv p1;
			r_uv p2;
			r_uv p3;
		};
		r_uv uvs[4];
	};
public:
	r_uv_rect() {}
	r_uv_rect(float left_u, float left_v, float top_u, float top_v, float right_u, float right_v, float bottom_u, float bottom_v) {
		p0.u = left_u;
		p0.v = left_v;
		p1.u = top_u;
		p1.v = top_v;
		p2.u = right_u;
		p2.v = right_v;
		p3.u = bottom_u;
		p3.v = bottom_v;
	}
	r_uv_rect(r_uv l, r_uv t, r_uv r, r_uv b) : p0(l), p1(t), p2(r), p3(b) {}
	~r_uv_rect() {}
};

class r_rect
{
	inline void update_right_and_bottom() {
		right = left + width;
		bottom = top + height;
	}

public:
	int left, top, right, bottom;
	int width, height;

	r_rect() : left(0), top(0), right(0), bottom(0), width(0), height(0) {}
	r_rect(int _x, int _y, int _width, int _height) : left(_x), top(_y), right(_x + _width), bottom(_y + _height),
		width(_width), height(_height) {}

	~r_rect() {}

	void set_position(int x, int y) {
		left = x;
		top = y;
		update_right_and_bottom();
	}

	void set_size(int w, int h) {
		width = w;
		height = h;
		update_right_and_bottom();
	}

	bool inside(r_pt &pt) { return left <= pt.x && top <= pt.y && right >= pt.x && bottom >= pt.y; }
	bool inside(r_rect &rect) { return left <= rect.left && top <= rect.top && right >= rect.right && bottom >= rect.bottom; }

	void uv_subrect(r_uv_rect &dst, r_rect subrect) {
		float fwidth = (float)width;
		float fheight = (float)height;
		dst.p0.u = subrect.left / fwidth;
		dst.p0.v = subrect.top / fheight;
		dst.p1.u = subrect.right / fwidth;
		dst.p1.v = subrect.top / fheight;
		dst.p2.u = subrect.right / fwidth;
		dst.p2.v = subrect.bottom / fheight;
		dst.p3.u = subrect.left / fheight;
		dst.p3.v = subrect.bottom / fheight;
	}
};