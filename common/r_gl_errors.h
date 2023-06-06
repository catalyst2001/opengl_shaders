#pragma once
#include <Windows.h>
#include <gl/GL.h>
#include <stdio.h>

extern int gl_current_error;
#define GL_CURRENT_ERROR() (gl_current_error)

#ifdef _GL_RENDERER_DEBUG_MODE
#define GL_CALL(func) \
	gl_current_error = glGetError(); \
	if(gl_current_error != GL_NO_ERROR) \
		fprintf(stderr, "[GL ERROR] Call " #func " failed with error code %d (0x%X)\n", gl_current_error, gl_current_error);

#else
#define GL_CALL(func)
#endif
