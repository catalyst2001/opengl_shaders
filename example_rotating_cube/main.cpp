#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#pragma comment(lib, "../common/glfw3/glfw3.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "legacy_stdio_definitions.lib")

// Координаты вершин кубика
GLfloat vertices[] = {
	-0.5f, -0.5f, -0.5f,
	0.5f, -0.5f, -0.5f,
	0.5f, 0.5f, -0.5f,
	-0.5f, 0.5f, -0.5f,
	-0.5f, -0.5f, 0.5f,
	0.5f, -0.5f, 0.5f,
	0.5f, 0.5f, 0.5f,
	-0.5f, 0.5f, 0.5f
};

// Индексы вершин для отрисовки граней кубика
GLuint indices[] = {
	0, 1, 2, 2, 3, 0,
	1, 5, 6, 6, 2, 1,
	7, 6, 5, 5, 4, 7,
	4, 0, 3, 3, 7, 4,
	4, 5, 1, 1, 0, 4,
	3, 2, 6, 6, 7, 3
};

// Шейдерный код для вершинного шейдера
const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;
    void main()
    {
        gl_Position = projection * view * model * vec4(aPos, 1.0);
    }
)";

// Шейдерный код для фрагментного шейдера
const char* fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;
    void main()
    {
        FragColor = vec4(1.0, 0.5, 0.2, 1.0);
    }
)";

// Функция для обработки нажатия клавиш
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
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
	GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Cube Demo", nullptr, nullptr);
	if (!window)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	// Инициализация GLEW
	if (!gladLoadGL())
	{
		std::cerr << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	// Создание вершинного шейдера
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
	glCompileShader(vertexShader);

	// Проверка статуса компиляции вершинного шейдера
	GLint success;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char infoLog[512];
		glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
		std::cerr << "Failed to compile vertex shader\n" << infoLog << std::endl;
		return -1;
	}

	// Создание фрагментного шейдера
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
	glCompileShader(fragmentShader);

	// Проверка статуса компиляции фрагментного шейдера
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char infoLog[512];
		glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
		std::cerr << "Failed to compile fragment shader\n" << infoLog << std::endl;
		return -1;
	}

	// Создание шейдерной программы и привязка шейдеров
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	// Проверка статуса привязки шейдеров
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		char infoLog[512];
		glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
		std::cerr << "Failed to link shader program\n" << infoLog << std::endl;
		return -1;
	}

	// Удаление шейдеров после привязки
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// Создание буфера вершин и индексов
	GLuint VBO, EBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// Привязка VAO
	glBindVertexArray(VAO);

	// Привязка буфера вершин
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Привязка буфера индексов
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Указание расположения вершинного атрибута
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);

	// Отключение привязки VAO
	glBindVertexArray(0);

	// Отключение привязки буфера вершин
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Отключение привязки буфера индексов
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Включение режима глубины
	glEnable(GL_DEPTH_TEST);

	// Основной цикл приложения
	while (!glfwWindowShouldClose(window))
	{
		// Обработка ввода
		processInput(window);

		// Очистка буфера цвета и глубины
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Активация шейдерной программы
		glUseProgram(shaderProgram);

		// Создание матрицы модели, вида и проекции
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);

		float time = (float)glfwGetTime();
		model = glm::rotate(model, time, glm::vec3(1.f, 0.f, 0.f));
		model = glm::rotate(model, time, glm::vec3(0.f, 1.f, 0.f));
		model = glm::rotate(model, time, glm::vec3(0.f, 0.f, 1.f));

		// Установка позиции камеры
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

		// Установка параметров проекции
		projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

		// Передача матриц в шейдер
		GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
		GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
		GLint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

		// Отрисовка кубика
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		// Проверка и обработка событий окна
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Освобождение ресурсов
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteProgram(shaderProgram);

	// Завершение работы GLFW
	glfwTerminate();

	return 0;
}