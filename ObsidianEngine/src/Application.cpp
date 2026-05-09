#include "Application.h"   
#include "Scene.h"          
#include "MeshComponent.h" 
#include "VulkanDevice.h"  
#include "Vertex.h"
#include "RenderSystem.h"
#include "ObsidianTime.h"

#include <vector>        
#include <cstdint>       
#include <cstring>       
#include <tuple>         
#include <vulkan/vulkan_raii.hpp>

void ObsidianEngine::Application::run()
{
	initWindow();
	initRender();
	m_activeScene = std::make_shared<Scene>();
	m_activeScene->addSystem<RenderSystem>(
		static_cast<VulkanDevice*>(m_renderDevice.get())
	);
	onStartup();

	//std::vector<Vertex> vertices = {
	//		{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
	//		{{ 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
	//		{{ 0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}},
	//		{{-0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}}
	//};

	//std::vector<uint16_t> indices = {
	//	0, 1, 2, 2, 3, 0
	//};

	//EntityID square = m_activeScene->createEntity();

	//m_activeScene->getRegistry().addComponent<MeshComponent>(square, vertices, indices, -1);

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
	static float timer = 0.0f;

	while (!m_window->shouldClose())
	{
		m_window->pollEvents();

		ObsidianTime::update(m_window->getTime());

		timer += ObsidianTime::deltaTime();
		if (timer >= 0.25f)
		{
			std::cout << "\rFPS: " << 1.0f / ObsidianTime::deltaTime() << "   " << std::flush;
			timer = 0.0f;
		}

		m_activeScene->onUpdate(ObsidianTime::deltaTime());

		onUpdate();
	}

	m_renderDevice->waitIdle();
}

void ObsidianEngine::Application::cleanup()
{

}