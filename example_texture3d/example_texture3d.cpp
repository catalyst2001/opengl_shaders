#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"

#pragma comment(lib, "../common/glfw3/glfw3.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "legacy_stdio_definitions.lib")

// Размеры окна
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

// Трехмерные текстуры
GLuint texture3D;

// Функция загрузки трехмер текстуры
void load3DTexture()
{
	// Генерация и привязка текстуры
	glGenTextures(1, &texture3D);
	glBindTexture(GL_TEXTURE_3D, texture3D);

	// Загрузка данных в текстуру
	// Здесь вы можете использовать свои данные или загрузить текстуру из файла
	// Пример загрузки данных в трехмерную текстуру
	int width = 64;
	int height = 64;
	int depth = 64;
	GLubyte* data = new GLubyte[width * height * depth * 4]; // RGBA формат

	// Заполнение данных текстуры
	for (int z = 0; z < depth; z++)
	{
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				int index = (z * width * height + y * width + x) * 4;
				// Здесь вы можете задать значения цвета или других атрибутов пикселя
				data[index] = 255; // Красный канал
				data[index + 1] = 0; // Зеленый канал
				data[index + 2] = 0; // Синий канал data[index + 3] = 255; // Альфа канал
			}
		}
	}

	// Установка параметров текстуры
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, width, height, depth, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	// Освобождение памяти
	delete[] data;
}

struct vec2 {
	vec2() : u(0.f), v(0.f) {}
	vec2(float xx, float yy) : u(xx), v(yy) {}
	~vec2() {}

	float u, v;
};

struct vec3 {
	vec3() : x(0.f), y(0.f), z(0.f) {}
	vec3(float xx, float yy, float zz) : x(xx), y(yy), z(zz) {}
	~vec3() {}

	float x, y, z;
};

struct vertex_s {
	vec3 vertex;
	vec3 normal;
	vec2 texcoord;
};

struct RAM_mesh_s {
	unsigned int num_indices;
	unsigned int num_vertices;
	unsigned int *p_indices;
	vertex_s *p_vertices;
};

bool create_cylinder(RAM_mesh_s *p_dst_mesh, float radius, float height, int slices)
{
	if (slices < 3)
		slices = 3;
	
	/* alloc vertices */
	p_dst_mesh->num_vertices = slices * 2;
	p_dst_mesh->p_vertices = (vertex_s *)malloc((size_t)p_dst_mesh->num_vertices);
	if (!p_dst_mesh->p_vertices)
		return false;

	for (int i = 0; i < 2; ++i) {
		float z = i == 0 ? height / 2 : -height / 2;
		for (int j = 0; j < slices; ++j) {
			vertex_s *p_curr_vert = &p_dst_mesh->p_vertices[i*slices + j];
			float angle = 2 * 3.14 * (float)j / (float)slices;
			float x = radius * cosf(angle);
			float y = radius * sinf(angle);
			p_curr_vert->vertex = vec3(x, y, z);		
			p_curr_vert->normal = vec3(cosf(angle), sinf(angle), 0.0);
			p_curr_vert->texcoord = vec2((float)j / (float)slices, i == 0 ? 0.f : 1.f);
		}
	}

	/* alloc indices */
	p_dst_mesh->num_indices = slices * 6;
	p_dst_mesh->p_indices = (unsigned int *)malloc((size_t)p_dst_mesh->num_indices);
	if (!p_dst_mesh->p_indices) {
		if (p_dst_mesh->p_vertices) {
			free(p_dst_mesh->p_vertices);
			p_dst_mesh->p_vertices = NULL;
		}
		free(p_dst_mesh->p_indices);
		return false;
	}

	for (int i = 0; i < slices; ++i) {
		int a = i;
		int b = slices + i;
		int c = slices + (i + 1) % slices;
		int d = (i + 1) % slices;
		
		p_dst_mesh->p_indices[i * 6 + 0] = a;
		p_dst_mesh->p_indices[i * 6 + 1] = b;
		p_dst_mesh->p_indices[i * 6 + 2] = c;
		p_dst_mesh->p_indices[i * 6 + 3] = a;
		p_dst_mesh->p_indices[i * 6 + 4] = c;
		p_dst_mesh->p_indices[i * 6 + 5] = d;
	}
	return true;
}

void delete_mesh(RAM_mesh_s *p_src_mesh)
{
	if (p_src_mesh->p_indices) {
		free(p_src_mesh->p_indices);
		p_src_mesh->p_indices = NULL;
	}

	if (p_src_mesh->p_vertices) {
		free(p_src_mesh->p_vertices);
		p_src_mesh->p_vertices = NULL;
	}
}

int main()
{
	// Инициализация GLFW
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return -1;
	}

	// Создание окна
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Texture Example", nullptr, nullptr);
	if (!window)
	{
		printf("Failed to create GLFW window");
		glfwTerminate();
		return -1;
	}

	// Установка контекста OpenGL
	glfwMakeContextCurrent(window);

	// Инициализация GLEW
	if (!gladLoadGL())
	{
		printf("Failed to initialize GLEW");
		glfwTerminate();
		return -1;
	}

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	char error[256];
	shader_program_t standard;
	uint32_t vertex_shader, fragment_shader;

	static const char *p_vsource = {
		""
		""
		""
		""
		""
		""
	};


	if (!gl_shader_source_compile_object(&vertex_shader, error, sizeof(error), GL_VERTEX_SHADER, )) {
		printf("Failed to link vertex shader\n");
	}


	RAM_mesh_s mesh;
	if (!create_cylinder(&mesh, 10, 20, 40)) {
		printf("Memory allocation error!\n");
		return 1;
	}

	GLuint buffers[2];
	glGenBuffers(2, buffers);
	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, mesh.num_vertices * sizeof(vertex_s), (const void *)mesh.p_vertices, GL_STATIC_DRAW);
	if (glGetError() != GL_OUT_OF_MEMORY) {
		printf("VBO allocation error!\n");
		return 1;
	}

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_s), (const void *)offsetof(vertex_s, vertex));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_s), (const void *)offsetof(vertex_s, normal));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_s), (const void *)offsetof(vertex_s, texcoord));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.num_indices * sizeof(unsigned int), (const void *)mesh.p_indices, GL_STATIC_DRAW);
	if (glGetError() != GL_OUT_OF_MEMORY) {
		printf("VBO allocation error!\n");
		return 1;
	}

	// Загрузка трехмерной текстуры
	load3DTexture();

	// Основной цикл программы
	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBindTexture(GL_TEXTURE_3D, texture3D);


		

		glDrawArrays(GL_TRIANGLES, 0, mesh.num_indices);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Освобождение ресурсов
	glDeleteTextures(1, &texture3D);
	glfwTerminate();

	return 0;
}
