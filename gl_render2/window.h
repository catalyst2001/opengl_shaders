#pragma once

#if defined LINUX
#include <X11/X.h>
#include <X11/Xlib.h>
#include <unistd.h>
#else
#include <Windows.h>
#endif

class cwindow;

class cwindowrect
{
	friend class cwindow;

	int left, top, right, bottom;
public:
	cwindowrect() : left(0), top(0), right(0), bottom(0) {}
	~cwindowrect() {}

	int get_x() { return left; }
	int get_y() { return top; }
	int get_width() { return right - left; }
	int get_height() { return bottom - top; }
};

class cwindow
{
	int xpos;
	int ypos;
	int width;
	int height;

#if defined LINUX
	Display *h_window;
#else
	HWND h_window;
#endif
public:
	cwindow();
	~cwindow();

	int create(int x, int y, int width, int height);
	int move_window(int x, int y);
	int size_window(int width, int height);
	int to_screen_center();
	
	template<typename _type>
	_type get_system_specific_window() { return *(_type *)&h_window; }
	
	void set_window_title(const char *p_text, ...);
	void get_window_title(char *p_dst, size_t maxlen);
	
	void get_window_rect(cwindowrect &dst_rect);

	int destrory();
};

void test()
{
	cwindow window;
	if (window.create(0, 0, 800, 600)) {
		window.get_system_specific_window<HWND>();


	}


}