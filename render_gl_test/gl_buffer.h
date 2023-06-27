#pragma once
#include <stddef.h>
#include <glad/glad.h>

enum GL_BUFFER_STATUS {
	BUFFER_STATUS_OK = 0,
	BUFFER_STATUS_INVALID_SIZE,
	BUFFER_STATUS_INVALID_BINDING,
	BUFFER_STATUS_OUT_OF_MEMORY,
	BUFFER_STATUS_UNKNOWN_ERROR
};

template<GLenum buffer_target, GLenum access_freqency, typename _type>
class gl_buffer
{
	GLsizei size;
	GLuint buffer;

	template<const GLenum target>
	static constexpr GLenum gl_get_binding_const_from_buffer_target()
	{
		if (target == GL_ARRAY_BUFFER)
			return GL_ARRAY_BUFFER_BINDING;

		if (target == GL_ELEMENT_ARRAY_BUFFER)
			return GL_ELEMENT_ARRAY_BUFFER_BINDING;

		if (target == GL_PIXEL_PACK_BUFFER)
			return GL_PIXEL_PACK_BUFFER_BINDING;

		if (target == GL_PIXEL_UNPACK_BUFFER)
			return GL_PIXEL_UNPACK_BUFFER_BINDING;

		static_assert(0, "Unknown target");
		return 0;
	}

	class gl_buffer_bind_saver
	{
		GLuint previous_buffer;
	public:
		gl_buffer_bind_saver(gl_buffer<buffer_target, access_freqency, _type> &buffer) {
			previous_buffer = 0;
			glGetIntegerv(gl_get_binding_const_from_buffer_target<buffer_target>(), &previous_buffer);
		}

		~gl_buffer_bind_saver() { glBindBuffer(buffer_target, previous_buffer); }
	};

	bool is_allocated() { return buffer != 0; }

	GLenum alloc_memory(GLuint &dst_buffer, GLsizei _size) {
		glGenBuffers(1, &dst_buffer);
		glBindBuffer(buffer_target, dst_buffer);
		glBufferData(buffer_target, _size * sizeof(_type), NULL, access_freqency);
		return glGetError();
	}

	GL_BUFFER_STATUS gl_buffer_error_to_buffer_status(GLenum error) {
		switch (error) {
		case GL_NO_ERROR:
			return BUFFER_STATUS_OK;

		case GL_INVALID_VALUE:
			return BUFFER_STATUS_INVALID_SIZE;

		case GL_INVALID_OPERATION:
			return BUFFER_STATUS_INVALID_BINDING;

		case GL_OUT_OF_MEMORY:
			return BUFFER_STATUS_OUT_OF_MEMORY;
		}
		return BUFFER_STATUS_UNKNOWN_ERROR;
	}

public:
	gl_buffer() {}
	~gl_buffer() {}

	GLenum get_buffer_type() { return buffer_target; }
	GLenum get_buffer_access_freqency() { return access_freqency; }

	GL_BUFFER_STATUS alloc(GLsizei _size) {
		GLenum error;
		gl_buffer_bind_saver bind_save(this);
		size = _size;
		if (!is_allocated()) {
			error = alloc_memory(buffer, _size * sizeof(_type));
			if (error != GL_NO_ERROR) {
				return gl_buffer_error_to_buffer_status(error);
			}		
		}
		return BUFFER_STATUS_OK; //OK
	}

	GL_BUFFER_STATUS realloc(GLsizei _size, bool b_copy_data = true) {
		GLenum error;
		GLuint new_buffer;
		gl_buffer_bind_saver bind_save(this);

		/* check buffer allocation */
		if (!is_allocated())
			return alloc(_size);

		/* copy old buffer data to new buffer */
		if (b_copy_data) {
			error = alloc_memory(new_buffer, _size * sizeof(_type))
			if (error != GL_NO_ERROR)
				return gl_buffer_error_to_buffer_status(error);

			/* GL 3.1 version specific */
			if (GLAD_GL_VERSION_3_1) {
				glBindBuffer(GL_COPY_READ_BUFFER, buffer);
				glBindBuffer(GL_COPY_WRITE_BUFFER, new_buffer);
				glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, size);
				glDeleteBuffers(1, &buffer);
			}

			/* 3.0 version */
			else {
				// copy from old buffer
				glBindBuffer(GL_COPY_READ_BUFFER, buffer);
				glBindBuffer(GL_COPY_WRITE_BUFFER, new_buffer);
				GLvoid *p_data = glMapBuffer(GL_COPY_READ_BUFFER, GL_READ_ONLY);
				glBufferSubData(GL_COPY_WRITE_BUFFER, 0, size, p_data);
				glUnmapBuffer(GL_COPY_READ_BUFFER);
			}
			buffer = new_buffer;
			size = _size;
			return;
		}
	}

	bool free() {
		if (buffer) {
			glDeleteBuffers(1, &buffer);
			buffer = 0;
		}
	}

	GLenum get_size(GLsizei *p_dst_size) {
		glGetBufferParameteriv(buffer_target, GL_BUFFER_SIZE, (GLint *)p_dst_size);
		return glGetError();
	}
};

void test()
{
	gl_buffer<GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW, float> buffer;
	if (buffer.alloc(10000) == BUFFER_STATUS_OK) {

	}
}

