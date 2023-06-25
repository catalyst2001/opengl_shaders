#pragma once
#include <stddef.h>
#include <glad/glad.h>

template<const GLenum target>
constexpr GLenum gl_get_binding_from_buffer_target()
{
	if constexpr (target == GL_ARRAY_BUFFER)
		return GL_ARRAY_BUFFER_BINDING;
	else if constexpr (target == GL_ELEMENT_ARRAY_BUFFER)
		return GL_ELEMENT_ARRAY_BUFFER_BINDING;
	else
		static_assert(0, "Unknown target");
}

template<GLenum buffer_type, GLenum access_freqency, typename _type>
class gl_buffer
{
	GLsizei size;
	GLuint buffer;
public:
	gl_buffer() {}
	~gl_buffer() {}

	GLenum get_buffer_type() { return buffer_type; }
	GLenum get_buffer_access_freqency() { return access_freqency; }

	GLenum alloc(GLsizei _size) {
		size = _size;
		if (!buffer)
			glGenBuffers(1, &buffer);

		glGetIntegerv(gl_get_binding_from_buffer_target<buffer_type>(), &previous_vbo);
		glBindBuffer(buffer_type, buffer);
		glBufferData(buffer_type, size * sizeof(_type), NULL, access_freqency);
		return glGetError();
	}

	GLenum realloc(GLsizei _size, bool b_copy_data = true) {
		GLenum error;
		GLsizei old_size;
		GLuint new_buffer;
		old_size = size;
		size = _size;
		if (buffer) {
			if (b_copy_data) {
				glGenBuffers(1, &new_buffer);
				glBufferData(buffer_type, size * sizeof(_type), NULL, access_freqency);
				error = glGetError();
				if (error != GL_NO_ERROR)
					return error;

				if (GLAD_GL_VERSION_3_1) {
					//glGenBuffers(1, &new_vbo);
					//glBindBuffer(GL_ARRAY_BUFFER, new_vbo);
					//glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)vertex_buffer.size(), NULL, GL_DYNAMIC_DRAW);

					//glBindBuffer(GL_COPY_READ_BUFFER, gui_vbo);
					//glBindBuffer(GL_COPY_WRITE_BUFFER, new_vbo);
					//glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, vbo_size);

					//glDeleteBuffers(1, &gui_vbo);
					//gui_vbo = new_vbo;
				}
				else {
					//glGenBuffers(1, &new_vbo);
					//glBindBuffer(GL_ARRAY_BUFFER, new_vbo);
					//glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)vertex_buffer.size(), NULL, GL_DYNAMIC_DRAW);

					//// copy from old buffer
					//glBindBuffer(GL_COPY_READ_BUFFER, gui_vbo);
					//glBindBuffer(GL_COPY_WRITE_BUFFER, new_vbo);
					//GLvoid* oldData = glMapBuffer(GL_COPY_READ_BUFFER, GL_READ_ONLY);
					//glBufferSubData(GL_COPY_WRITE_BUFFER, 0, vbo_size, oldData);
					//glUnmapBuffer(GL_COPY_READ_BUFFER);
				}

				return;
			}
		}
	}

	bool free() {
		if (buffer) {
			glDeleteBuffers(1, &buffer);
			buffer = 0;
		}
	}

	GLenum get_size(GLsizei *p_dst_size) {
		glGetBufferParameteriv(buffer_type, GL_BUFFER_SIZE, (GLint *)p_dst_size);
		return glGetError();
	}
};

void test()
{
	gl_buffer<GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW, float> buffer;

	buffer.get_buffer_access_freqency();
}

