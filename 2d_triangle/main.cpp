#include <stdio.h>
#include <iostream>

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "../common/glfw3/glfw3.lib")
#pragma comment(lib, "legacy_stdio_definitions.lib")
#include "../common/glad/glad.h"
#include <Windows.h>
#include <gl/GL.h>
#include "../common/glfw3/GLFW/glfw3.h"
#define WIDTH 800
#define HEIGHT 600

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct gui_vertex_s {
  glm::vec2 vertex;
  glm::vec2 uv;
  glm::vec4 color;
};

GLuint createShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource) {
  // Создание вершинного шейдера
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);

  // Проверка статуса компиляции вершинного шейдера
  GLint success;
  GLchar infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertexShader, sizeof(infoLog), NULL, infoLog);
    std::cerr << "Vertex shader compilation failed: " << infoLog << std::endl;
  }

  // Создание фрагментного шейдера
  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);

  // Проверка статуса компиляции фрагментного шейдера
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragmentShader, sizeof(infoLog), NULL, infoLog);
    std::cerr << "Fragment shader compilation failed: " << infoLog << std::endl;
  }

  // Создание программы шейдеров
  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  // Проверка статуса линковки программы
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shaderProgram, sizeof(infoLog), NULL, infoLog);
    std::cerr << "Shader program linking failed: " << infoLog << std::endl;
  }

  // Удаляем вершинный и фрагментный шейдеры после линковки
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  return shaderProgram;
}

int main() {
  // Инициализация GLFW
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW" << std::endl;
    return -1;
  }

  // Создание окна
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Triangle", NULL, NULL);
  if (!window) {
    std::cerr << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);

  // Инициализация GLEW
  if (!gladLoadGL()) {
    std::cerr << "Failed to initialize GLEW" << std::endl;
    return -1;
  }

  // Вершинный шейдер
  const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec2 inVertex;
        layout (location = 1) in vec2 inUV;
        layout (location = 2) in vec4 inColor;

        uniform mat4 model;
        out vec2 uv;
        out vec4 color;

        void main() {
            gl_Position = model * vec4(inVertex, 0.0, 1.0);
            uv = inUV;
            color = inColor;
        }
    )";

  // Фрагментный шейдер
  const char* fragmentShaderSource = R"(
        #version 330 core
        in vec2 uv;
        in vec4 color;
        out vec4 fragColor;

        void main() {
            fragColor = color;
        }
    )";

  // Создание и компиляция шейдеров, создание программы шейдеров
  GLuint shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);

  // Определение вершин треугольника
  gui_vertex_s vertices[] = {
      {{-0.5f, -0.5f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
      {{0.5f, -0.5f}, {1.0f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},
      {{0.0f, 0.5f}, {0.5f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f}}
  };

  // Создание и настройка буфера вершин и массива вершин
  GLuint VBO, VAO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(gui_vertex_s), (void*)offsetof(gui_vertex_s, vertex));
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(gui_vertex_s), (void*)offsetof(gui_vertex_s, uv));
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(gui_vertex_s), (void*)offsetof(gui_vertex_s, color));
  glEnableVertexAttribArray(2);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  // Основной цикл рендеринга
  while (!glfwWindowShouldClose(window)) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Использование программы шейдеров
    glUseProgram(shaderProgram);

    // Передача матрицы преобразования в шейдер
    glm::mat4 model = glm::mat4(1.0f);
    GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    // Рендеринг треугольника
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // Проверка и обработка событий окна
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // Освобождение ресурсов
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteProgram(shaderProgram);

  // Завершение работы GLFW
  glfwTerminate();

  return 0;
}
