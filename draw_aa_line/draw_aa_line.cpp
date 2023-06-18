#include <stdio.h>
#pragma comment(lib, "../common/glfw3/glfw3.lib")
#pragma comment(lib, "legacy_stdio_definitions.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#include <Windows.h>
#include <gl/GL.h>
#include <gl/glu.h>
#include "../common/glfw3/GLFW/glfw3.h"
#include <cmath>
#include <vector>

#define WIDTH 800
#define HEIGHT 600

void error_callback(int error_code, const char* description)
{
	printf("GLFW error: '%s' (%d)\n", description, error_code);
}

void drawLine(float x1, float y1, float x2, float y2, float width)
{
	// Вычисляем вектор нормали к сегменту
	float dx = x2 - x1;
	float dy = y2 - y1;
	float len = sqrtf(dx*dx + dy * dy);
	dx /= len;
	dy /= len;
	float nx = -dy;
	float ny = dx;

	// Вычисляем координаты вершин прямоугольника
	float w2 = width / 2.0f;
	float x3 = x1 + nx * w2;
	float y3 = y1 + ny * w2;
	float x4 = x2 + nx * w2;
	float y4 = y2 + ny * w2;
	float x5 = x2 - nx * w2;
	float y5 = y2 - ny * w2;
	float x6 = x1 - nx * w2;
	float y6 = y1 - ny * w2;

	// Рисуемямоугольник
	glBegin(GL_QUADS);
	glVertex2f(x3, y3);
	glVertex2f(x4, y4);
	glVertex2f(x5, y5);
	glVertex2f(x6, y6);
	glEnd();

	// Рисуем линию
	glBegin(GL_LINES);
	glVertex2f(x1, y1);
	glVertex2f(x2, y2);
	glEnd();
}

void drawLineWu(float x1, float y1, float x2, float y2, float width)
{
	// Вычисляем вектор нормали к сегменту
	float dx = x2 - x1;
	float dy = y2 - y1;
	float len = sqrt(dx*dx + dy * dy);
	dx /= len;
	dy /= len;
	float nx = -dy;
	float ny = dx;

	// Вычисляем координаты вершин прямугольника
	float w2 = width / 2.0f;
	float x3 = x1 + nx * w2;
	float y3 = y1 + ny * w2;
	float x4 = x2 + nx * w2;
	float y4 = y2 + ny * w2;
	float x5 = x2 - nx * w2;
	float y5 = y2 - ny * w2;
	float x6 = x1 - nx * w2;
	float y6 = y1 - ny * w2;

	glBegin(GL_QUADS);
	glVertex2f(x3, y3);
	glVertex2f(x4, y4);
	glVertex2f(x5, y5);
	glVertex2f(x6, y6);
	glEnd();

	// Рисуем линию
	glBegin(GL_LINES);
	glVertex2f(x1, y1);
	glVertex2f(x2, y2);
	glEnd();

	// Рисуем сглаженные края
	float x = 0.f, y = 0.f, grad;
	if (fabs(dx) > fabs(dy)) {
		if (x1 > x2) {
			std::swap(x1, x2);
			std::swap(y1, y2);
		}
		grad = dy / dx;
		y = y1 + grad * (x - x1);
		for (x = x1 + 1; x < x2; x++) {
			float alpha = 1.0f - (x - x1) / (x2 - x1);
			glColor4f(1.0f, 1.0f, 1.0f, alpha);
			glBegin(GL_POINTS);
			glVertex2f(x, floor(y));
			glVertex2f(x, ceil(y));
			glEnd();
			y += grad;
		}
	}
	else {
		if (y1 > y2) {
			std::swap(x1, x2);
			std::swap(y1, y2);
		}
		grad = dx / dy;
		x = x1 + grad * (y - y1);
		for (y = y1 + 1; y < y2; y++) {
			float alpha = 1.0f - (y - y1) / (y2 - y1);
			glColor4f(1.0f, 1.0f, 1.0f, alpha);
			glBegin(GL_POINTS);
			glVertex2f(floor(x), y);
			glVertex2f(ceil(x), y);
			glEnd();
			x += grad;
		}
	}
}

void drawLineOversampled(float x1, float y1, float x2, float y2, float width, int oversampling)
{
	// Вычисляем вектор нормали к сегменту
	float dx = x2 - x1;
	float dy = y2 - y1;
	float len = sqrtf(dx*dx + dy * dy);
	dx /= len;
	dy /= len;
	float nx = -dy;
	float ny = dx;

	// Вычисляем координаты вершин прямоольника
	float w2 = width / 2.0f;
	float x3 = x1 + nx * w2;
	float y3 = y1 + ny * w2;
	float x4 = x2 + nx * w2;
	float y4 = y2 + ny * w2;
	float x5 = x2 - nx * w2;
	float y5 = y2 - ny * w2;
	float x6 = x1 - nx * w2;
	float y6 = y1 - ny * w2;

	// Вычисляем количество сэмплов
	int numSamples = oversampling * len;

	// Рисуем линию с более высоким разрешением
	glBegin(GL_TRIANGLE_STRIP);
	for (int i = 0; i <= numSamples; i++) {
		float t = (float)i / (float)numSamples;
		float x = x1 + dx * len * t;
		float y = y1 + dy * len * t;
		float alpha = 1.0f - fabs(2.0f * t - 1.0f);
		glColor4f(1.0f, 1.0f, 1.0f, alpha);
		glVertex2f(x + nx * w2, y + ny * w2);
		glVertex2f(x - nx * w2, y - ny * w2);
	}
	glEnd();
}

int main()
{
	glfwInit();
	glfwSetErrorCallback(error_callback);

	GLFWwindow *p_window;
	GLFWmonitor *p_monitor = /*glfwGetPrimaryMonitor()*/NULL;
	p_window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL demo application", p_monitor, NULL);
	if (!p_window) {
		printf("Failed to create window!\n");
		return 1;
	}
	glfwMakeContextCurrent(p_window);
	glfwSwapInterval(60);

	int width, height;
	glfwGetWindowSize(p_window, &width, &height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, (double)width, 0, (double)height);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	while (!glfwWindowShouldClose(p_window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();

		double curr_time = glfwGetTime();
		float sf = (float)fabs(sin(curr_time * 0.1));
		float cf = (float)fabs(cos(curr_time * 0.1));
		drawLineOversampled(0.f, 0.f, 600.f * sf, 600.f * cf, 2.f, 20);

		glfwSwapBuffers(p_window);
		glfwPollEvents();
	}

	glfwDestroyWindow(p_window);
	glfwTerminate();
	return 0;
}