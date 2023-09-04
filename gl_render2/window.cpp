#include "window.h"

bool b_initialized_window_subsystem = false;

cwindow::cwindow()
{
#if defined LINUX

#else
	//WNDCLASSEXA 
#endif
}

cwindow::~cwindow()
{
#if defined LINUX

#else

#endif
}

int cwindow::create(int x, int y, int width, int height)
{
#if defined LINUX

#else
	//h_window = CreateWindowExA(0, )
#endif
	return 0;
}

int cwindow::move_window(int x, int y)
{
	return 0;
}

int cwindow::size_window(int width, int height)
{
	return 0;
}

int cwindow::to_screen_center()
{
	return 0;
}

void cwindow::set_window_title(const char * p_text, ...)
{
}

void cwindow::get_window_title(char * p_dst, size_t maxlen)
{
}

void cwindow::get_window_rect(cwindowrect &dst_rect)
{
	RECT rect;
	GetWindowRect(h_window, &rect);
	dst_rect.left = rect.left;
	dst_rect.top = rect.top;
	dst_rect.right = rect.right;
	dst_rect.bottom = rect.bottom;
}

int cwindow::destrory()
{


	return 0;
}
