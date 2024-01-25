#include <stdio.h>
#pragma comment(lib, "../common/glfw3/glfw3.lib")
#pragma comment(lib, "legacy_stdio_definitions.lib")
#include "../common/glad/glad.h"
#include <Windows.h>
#include <gl/GL.h>
#include "../common/glfw3/GLFW/glfw3.h"
#include "../common/shader.h"

#define WIDTH 800
#define HEIGHT 600

void error_callback(int error_code, const char* description)
{
	printf("GLFW error: '%s' (%d)\n", description, error_code);
}

shader_program_t program;


GLuint vao;

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

int main()
{
	sizeof(uintptr_t);

	glfwInit();
	glfwSetErrorCallback(error_callback);




	GLFWwindow *p_window;
	GLFWmonitor *p_monitor = /*glfwGetPrimaryMonitor()*/NULL;
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	p_window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL demo application", p_monitor, NULL);
	if (!p_window) {
		printf("Failed to create window!\n");
		return 1;
	}
	glfwMakeContextCurrent(p_window);
	glfwSwapInterval(60);

	gladLoadGL();

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	const char *p_vertex_source =
		//"#version 120\n"
		"uniform"
		"uniform"
		""
		"void main()"
		"{"
		""
		""
		""
		""
		""
		"}";

	char error[512];
	uint32_t shaders[2];
	if (!gl_shader_source_compile_object(&shaders[0], error, sizeof(error), GL_VERTEX_SHADER, p_vertex_source)) {
		printf("gl_shader_source_compile_object vertex_shader: %s\n", error);
	}

	if (!gl_shader_source_compile_object(&shaders[1], error, sizeof(error), GL_FRAGMENT_SHADER, p_vertex_source)) {
		printf("gl_shader_source_compile_object fragment_shader: %s\n", error);
	}

	if (gl_shader_prog_create_program(&program, error, sizeof(error), 0, shaders, sizeof(shaders) / sizeof(shaders[0])) != SHADER_PROG_STATUS_OK) {
		printf("gl_shader_prog_create_program: %s\n", error);
	}

	while (!glfwWindowShouldClose(p_window)) {
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		
		

		
		glfwSwapBuffers(p_window);
		glfwPollEvents();
	}

	glfwDestroyWindow(p_window);
	glfwTerminate();
	return 0;
}