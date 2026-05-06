#include "../include/Application.h"

#include "../include/VulkanDevice.h"

void ObsidianEngine::Application::run()
{
	initWindow();
	initRender();
	mainLoop();
	cleanup();
}

void ObsidianEngine::Application::setFrameBufferRsizedFlag()
{
	m_renderDevice->m_framebufferResized = true;
}

void ObsidianEngine::Application::initWindow()
{
	m_window = std::make_unique<Window>(WIDTH, HEIGHT, "Obsidian Engine", this);
}

void ObsidianEngine::Application::initRender()
{
	m_renderDevice = std::make_unique<VulkanDevice>();
	m_renderDevice->init(m_window.get());
}

void ObsidianEngine::Application::mainLoop()
{
	while (!m_window->shouldClose())
	{
		m_window->pollEvents();

		m_renderDevice->beginFrame();
		m_renderDevice->drawTriangle();
		m_renderDevice->endFrame();

	}

	m_renderDevice->waitIdle();
}

void ObsidianEngine::Application::cleanup()
{

}