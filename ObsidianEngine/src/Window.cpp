#include "../include/Window.h"
#include "../include/Application.h"

#include <stdexcept>

namespace ObsidianEngine
{

	static void framebufferResizeCallback(GLFWwindow* window, int width, int height)
	{
		auto app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
		app->setFrameBufferRsizedFlag();
	}

	Window::Window(int width, int height, const std::string& name, Application* app) : m_width(width), m_height(height)
	{
		if (!glfwInit())
		{
			throw std::runtime_error("Failed to initalize GLFW!");
		}

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		m_window = glfwCreateWindow(m_width, m_height, name.c_str(), nullptr, nullptr);

		glfwSetWindowUserPointer(m_window, app);
		glfwSetFramebufferSizeCallback(m_window, framebufferResizeCallback);

		if (!m_window) {
			throw std::runtime_error("Failed to create GLFW window!");
		}
	}

	Window::~Window()
	{
		glfwDestroyWindow(m_window);
		glfwTerminate();
	}
}