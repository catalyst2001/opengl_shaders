#pragma once
#if defined LINUX
#include <x11.h>

#else
#include <Windows.h>
#include <gl/GL.h>
#include <gl/glu.h>

#endif

#define WINDOW_ID "RGEwindow"

class gl_context
{
#if defined LINUX
	// define x11 fields there
#else
	HWND h_window;
	HDC h_devctx;
	HGLRC h_glctx;
#endif


public:
	gl_context();
	~gl_context();

	int create_window();

	void make_context_current();

	/* multisampling */
	void set_multisampling_active(bool b_active);
	void set_multisampling_samples(int nsamples);

	void set_vsync_active(bool);

};

