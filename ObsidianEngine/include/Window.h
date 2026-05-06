#ifndef WINDOW_H_
#define WINDOW_H_

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>

namespace ObsidianEngine
{
	class Application;

	static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

	class Window 
	{
	public: 
		Window(int width, int height, const std::string& name, Application* app);
		~Window();

		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;

		bool shouldClose() const { return glfwWindowShouldClose(m_window); }
		void pollEvents() const { glfwPollEvents(); }
		void waitEvents() const { glfwWaitEvents(); }

		GLFWwindow* getNativeWindow() const { return m_window; }

		void getFramebufferSize(int* width, int* height) const { glfwGetFramebufferSize(m_window, width, height); }

	private:
		GLFWwindow* m_window;
		int m_width;
		int m_height;
	};
}

#endif