#include <stdio.h>
#pragma comment(lib, "../common/glfw3/glfw3.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "legacy_stdio_definitions.lib")
#include "../common/glad/glad.h"
#include <Windows.h>
#include <gl/GL.h>
#include "../common/glfw3/GLFW/glfw3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"

#include "r_gl_errors.h"

#include "gui/r_gui_draw.h"

#define WIDTH 800
#define HEIGHT 600

void error_callback(int error_code, const char* description)
{
	printf("GLFW error: '%s' (%d)\n", description, error_code);
}

/*
 We introduce two VAOs, for different types of geometry,
 and therefore for a different set of vertex attributes
*/
enum VAO_TYPES {
	VAO_3D = 0,
	VAO_2D,
	VAO_COUNT
};

GLuint vertex_array_objects[VAO_COUNT];

void glfwEnumMonitors()
{
	int count_monitors;
	GLFWmonitor **p_monitors = glfwGetMonitors(&count_monitors);
	for (int i = 0; i < count_monitors; i++) {
		printf("Montior %d\n", i);
		printf(" %s\n", glfwGetMonitorName(p_monitors[i]));
		int count_videomodes;

		const GLFWvidmode *p_videomodes = glfwGetVideoModes(p_monitors[i], &count_videomodes);
		for (size_t j = 0; j < count_videomodes; j++) {
			printf("   mode %d: %dx%d@%d\n", j, p_videomodes[j].width, p_videomodes[j].height, p_videomodes[j].refreshRate);
		}
	}
}

r_gui_draw_list drawlist;

glm::mat4x4 projection;
glm::mat4x4 view;
shader_program_t default_2d_shader;

void print_matrix(float *p_mat, int r, int c)
{
	for (int i = 0; i < r; i++) {
		for (int j = 0; j < c; j++) {
			printf("%f ", p_mat[j * 4 + i]);
		}
		printf("\n");
	}
}

GLuint LoadShader(GLenum Type, const char *p_source)
{
	GLuint Shader = glCreateShader(Type);
	glShaderSource(Shader, 1, (const char**)&p_source, NULL);
	glCompileShader(Shader);

	int CompileStatus;
	glGetShaderiv(Shader, GL_COMPILE_STATUS, &CompileStatus);
	if (CompileStatus == GL_FALSE) {
		int InfoLogLength = 0;
		glGetShaderiv(Shader, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0) {
			char *InfoLog = new char[InfoLogLength];
			int CharsWritten = 0;
			glGetShaderInfoLog(Shader, InfoLogLength, &CharsWritten, InfoLog);
			printf("SHADER INFO LOG: %s\n", InfoLog);
		}
		glDeleteShader(Shader);
		return 0;
	}
	return Shader;
}

bool create_default_shader()
{
	char error[512];
	uint32_t shaders[2];

	if (glGetError() == GL_INVALID_OPERATION)
		__asm int 3h;

	/* vertex shader */
	static const char *p_vertex_shader_source = {
		"#version 330 core\n"
		"layout (location = 0) in vec2 vertex;\n" // 0 attribute in VBO
		"layout (location = 1) in vec2 texcoord;\n" // 1 attribute in VBO
		"layout (location = 2) in vec4 color;\n" // 2 attribute in VBO
		
		"out vec2 in_frag_texcoord;\n" // to fragment shader
		"out vec4 in_frag_color;\n" // to fragment shader

		"uniform mat4 projection;\n"

		"void main()\n"
		"{\n"
			"in_frag_texcoord = texcoord;\n"
			"in_frag_color = color;\n"
			"gl_Position = projection * vec4(vertex, 0.0f, 1.0f);\n"
		"}"
	};

	/* fragment shader */
	static const char *p_fragment_shader_source = {
		"#version 330 core\n"
		"in vec2 in_frag_texcoord;\n"
		"in vec4 in_frag_color;\n"
		"out vec4 out_frag_color;\n"
		"uniform sampler2D tex_sampler;\n"

		"void main()\n"
		"{\n"
			//"gl_FragColor = texture(tex_sampler, in_frag_texcoord);\n"
			"gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);\n"
		"}"
	};

	shaders[0] = LoadShader(GL_VERTEX_SHADER, p_vertex_shader_source);
	shaders[1] = LoadShader(GL_FRAGMENT_SHADER, p_fragment_shader_source);

	default_2d_shader.program = glCreateProgram();
	if (default_2d_shader.program == 0) {
		printf("Failed to create shader program!\n");
		return 1;
	}

	glAttachShader(default_2d_shader.program, shaders[0]);
	glAttachShader(default_2d_shader.program, shaders[1]);
	glLinkProgram(default_2d_shader.program);

	GLint status;
	glGetProgramiv(default_2d_shader.program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		char error[1024];
		GLint log_length;
		glGetProgramiv(default_2d_shader.program, GL_INFO_LOG_LENGTH, &log_length);
		glGetProgramInfoLog(default_2d_shader.program, sizeof(error), NULL, error);
		printf("LINK PROGRAM ERROR\n%s\n", error);
		return 1;
	}


	//if (!gl_shader_source_compile_object(&shaders[0], error, sizeof(error), GL_VERTEX_SHADER, p_vertex_shader_source)) {
	//	printf("\nGL_VERTEX_SHADER ERROR\n%s\n", error);
	//	return false;
	//}

	//if (!gl_shader_source_compile_object(&shaders[1], error, sizeof(error), GL_FRAGMENT_SHADER, p_fragment_shader_source)) {
	//	printf("\nGL_VERTEX_SHADER ERROR\n%s\n", error);
	//	return false;
	//}

	//SHADER_PROG_STATUS prog_status = gl_shader_prog_create_program(&default_2d_shader, error, sizeof(error), 0, shaders, 2);
	//if (prog_status != SHADER_PROG_STATUS_OK) {
	//	printf("Failed to link program! Error code: %d\n", prog_status);
	//	return false;
	//}

	return true;
}

void APIENTRY gl_debug_proc(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar *message,
	const void *userParam)
{
	printf("[GL DEBUG] type(%d) id(%d) severity(%d) '%s'\n", type, id, severity, message);
}

void debug()
{
	//GLint flags;
	//glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	//if(flags & GL_CONTEXT_FLAG_DEBUG_BIT)

//	typedef void (APIENTRY *DEBUGPROC)(GLenum source,
//		GLenum type,
//		GLuint id,
//		GLenum severity,
//		GLsizei length,
//		const GLchar *message,
//		const void *userParam);
//
//#define GL_DEBUG_OUTPUT 0x92E0
//	typedef void (APIENTRY *glDebugMessageCallbackPfn)(DEBUGPROC callback, const void *userParam);
//
//	typedef void (APIENTRY *glDebugMessageControlPfn)(GLenum source,
//		GLenum type,
//		GLenum severity,
//		GLsizei count,
//		const GLuint *ids,
//		GLboolean enabled);
//
//	glDebugMessageCallbackPfn glDebugMessageCallback = (glDebugMessageCallbackPfn)glfwGetProcAddress("glDebugMessageCallback");
//	glDebugMessageControlPfn glDebugMessageControl = (glDebugMessageControlPfn)glfwGetProcAddress("glDebugMessageControl");
//	if (glDebugMessageCallback && glDebugMessageControl) {
//		glEnable(GL_DEBUG_OUTPUT);
//		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
//		glDebugMessageCallback(gl_debug_proc, NULL);
//	}
}

int main()
{
	glfwInit();
	glfwSetErrorCallback(error_callback);

	GLFWwindow *p_window;
	GLFWmonitor *p_monitor = /*glfwGetPrimaryMonitor()*/NULL;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);
	p_window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL demo application", p_monitor, NULL);
	if (!p_window) {
		printf("Failed to create window!\n");
		return 1;
	}
	glfwMakeContextCurrent(p_window);

	// Turn on vertical screen sync under Windows.
	// (I.e. it uses the WGL_EXT_swap_control extension)
	typedef BOOL(WINAPI *PFNWGLSWAPINTERVALEXTPROC)(int interval);
	PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = NULL;
	wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)glfwGetProcAddress("wglSwapIntervalEXT");
	if (wglSwapIntervalEXT)
		wglSwapIntervalEXT(1);

	gladLoadGL();

	GL_CALL(glGenVertexArrays(VAO_COUNT, vertex_array_objects));
	GL_CALL(glBindVertexArray(vertex_array_objects[VAO_2D]));
	drawlist.init();
	drawlist.clear();

	drawlist.set_texture(1);
	r_gui_vertex verts[] = {
		{{0, 0}, {0, 0}, {255, 0, 0}},
		{{100, 0}, {0, 0}, {0, 255, 0}},
		{{100, 100}, {0, 0}, {0, 0, 255}},
		{{0, 100}, {0, 0}, {255, 255, 0}},
	};
	drawlist.push_rect(verts);

	drawlist.set_texture(2);
	r_gui_vertex verts2[] = {
		{{500+0, 500 + 0}, {0, 0}, {255, 0, 0}},
		{{500+100, 500 + 0}, {0, 0}, {0, 255, 0}},
		{{500+100, 500 + 100}, {0, 0}, {0, 0, 255}},
		{{500+0, 500 + 100}, {0, 0}, {255, 255, 0}},
	};
	drawlist.push_rect(verts2);
	drawlist.commit();

	r_gui_draw_list::drawcmd_info info;
	drawlist.query_drawcmd_info(info);

	info.get_verts_data_offset();


	view = glm::mat4x4(1.f);

	if (!create_default_shader()) {
		printf("Failed to create default shader\n");
		getchar();
		return 1;
	}

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	GLuint projection_uniform = glGetUniformLocation(default_2d_shader.program, "projection");

	while (!glfwWindowShouldClose(p_window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		view = glm::mat4x4(1.f);

		int width, height;
		glfwGetWindowSize(p_window, &width, &height);
		projection = glm::ortho(0.f, (float)width, 0.f, (float)height);

		/* draw 2d objects */
		glBindVertexArray(vertex_array_objects[VAO_2D]);
		glUseProgram(default_2d_shader.program);
		glUniformMatrix4fv(projection_uniform, 1, GL_FALSE, glm::value_ptr(projection));

		drawlist.draw();

		/* draw 3d objects */
		//glBindVertexArray(vertex_array_objects[VAO_3D]);

		glfwSwapBuffers(p_window);
		glfwPollEvents();
	}

	glfwDestroyWindow(p_window);
	glfwTerminate();
	return 0;
}