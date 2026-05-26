#ifndef GL_DEVICE_H_
#define GL_DEVICE_H_

#include "RenderDevice.h"

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "Window.h"
#include "Vector.h"

#include "glslconv.h"

#include <iostream>
#include <vector>

static char shader[] = R"(
#version glslconv 

attribute vec2 aPos;
attribute vec3 aCol;

varying vec3 vCol;

#vertex 
vec4 Vertex()
{
	vCol = aCol;
	return vec4(aPos, 0, 1);
}	

#fragment
vec4 Fragment()
{
	return vec4(vCol, 1);
}
	
)";

namespace ObsidianEngine
{
	struct Vertex2
	{
		Vector2 pos;
		Vector3 color;
	};

	const std::vector<Vertex2> vertices2 = {
	{{0.0f, -0.5f}, {1.0f, 1.0f, 1.0f}},
	{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
	{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
	};

	static void CheckShaderErrors(GLuint shader) {
		GLint success;
		GLchar infoLog[1024];

		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			GLint logLength;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << infoLog << std::endl;
		}
	}

	class GLDevice : public IRenderDevice
	{
	public:
		Window* m_window;
		uint32_t m_shader;

		uint32_t vao, vbo;

		void init(Window* window)
		{
			m_window = window;

			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
			glfwSwapInterval(0);

			glfwMakeContextCurrent(window->getNativeWindow());

			if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) 
			{
				return;
			}

			glslconv_output_t out = glslconv_compile_330(shader, "./test.glsl");
			
			std::cout << out.fragment << std::endl;

			unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vertexShader, 1, (const char**)&out.vertex, NULL);
			glCompileShader(vertexShader);
			CheckShaderErrors(vertexShader);

			unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fragmentShader, 1, (const char**)&out.fragment, NULL);
			glCompileShader(fragmentShader);
			CheckShaderErrors(fragmentShader);

			m_shader = glCreateProgram();
			glAttachShader(m_shader, vertexShader);
			glAttachShader(m_shader, fragmentShader);
			glLinkProgram(m_shader);

			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);

			glGenVertexArrays(1, &vao);
			glGenBuffers(1, &vbo);

			glBindVertexArray(vao);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);

			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2[0]) * vertices2.size(), vertices2.data(), GL_STATIC_DRAW);
			int loc1 = glGetAttribLocation(m_shader, "aPos");
			int loc2 = glGetAttribLocation(m_shader, "aCol");
			glVertexAttribPointer(loc1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2), 0);
			glVertexAttribPointer(loc2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex2), reinterpret_cast<void*>(2 * sizeof(float)));
			glEnableVertexAttribArray(loc1);
			glEnableVertexAttribArray(loc2);
		}

		void beginFrame() 
		{
			glClearColor(0, 0, 0, 1);
			glClear(GL_COLOR_BUFFER_BIT);
		}

		void endFrame() 
		{
			glfwSwapBuffers(m_window->getNativeWindow());
		}

	    void drawTriangle() 
		{
			glUseProgram(m_shader);
			glBindVertexArray(vao);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);

			glDrawArrays(GL_TRIANGLES, 0, 3);
		}

		void waitIdle() 
		{

		}
	};
}

#endif