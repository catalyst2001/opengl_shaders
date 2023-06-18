#pragma once
#include "glad/glad.h"

extern int gl_current_error;
#define GL_CURRENT_ERROR() (gl_current_error)

#ifdef _GL_RENDERER_DEBUG_MODE
#include <intrin.h>

const char *gl_error_enum_to_string(GLenum error);

#define GL_CALL(func) \
	func;\
	gl_current_error = glGetError(); \
	if(gl_current_error != GL_NO_ERROR) { \
		fprintf(stderr, "[GL ERROR] Call " #func " failed with error (%s) code %d (0x%X)\n", gl_error_enum_to_string(gl_current_error), gl_current_error, gl_current_error); \
		__debugbreak(); \
	}

#define GL_CHECK_ERROR() \
	gl_current_error = glGetError(); \
	if(gl_current_error != GL_NO_ERROR) { \
		fprintf(stderr, "[GL ERROR] Current GL error in function " __FUNCTION__ " (%s) code %d (0x%X)\n", gl_error_enum_to_string(gl_current_error), gl_current_error, gl_current_error); \
		__debugbreak(); \
	}

#else
#define GL_CALL(func)
#define GL_CHECK_ERROR()
#endif
