#include <stdio.h>
#pragma comment(lib, "../common/glfw3/glfw3.lib")
#pragma comment(lib, "legacy_stdio_definitions.lib")
#include "../common/glfw3/GLFW/glfw3.h"
#include "../common/shader.h"

#define WIDTH 800
#define HEIGHT 600

void error_callback(int error_code, const char* description)
{
	printf("GLFW error: '%s' (%d)\n", description, error_code);
}

shader_program_t program;

int main()
{
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

	const char *p_vertex_source =
		"#version "
		""
		""
		""
		""
		"";

	char error[512];
	shader_info_t shaders[] = {
		{ SHADER_TYPE_VERTEX, },
		{ SHADER_TYPE_FRAGMENT, }
	};

	gl_shader_prog_create_program(&program, error, sizeof(error), 0,)

	while (!glfwWindowShouldClose(p_window)) {
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();



		
		glfwSwapBuffers(p_window);
		glfwPollEvents();
	}

	glfwDestroyWindow(p_window);
	glfwTerminate();
	return 0;
}