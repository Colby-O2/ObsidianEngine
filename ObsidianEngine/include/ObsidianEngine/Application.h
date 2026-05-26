#ifndef APPLICATION_H_
#define APPLICATION_H_

#include "Window.h"
#include "RenderDevice.h"
#include "ECS/Scene.h"

#include <iostream>
#include <memory>
#include <stdexcept>
#include <cstdlib>
#include<array>

namespace ObsidianEngine
{
	constexpr uint32_t WIDTH = 800;
	constexpr uint32_t HEIGHT = 600;

	class Application {
	public:
		Application() = default;
		virtual ~Application() = default;

		Application(const Application&) = delete;
		Application& operator=(const Application&) = delete;

		void run();
		void close() { m_running = false; }

		void setScene(std::shared_ptr<Scene> scene);
		Scene* getScene();

		void setFrameBufferRsizedFlag();

	protected:
		std::shared_ptr<Scene> m_activeScene;

		virtual void onStartup() {}
		virtual void onUpdate() {}
		virtual void onEvent() {}

	private:
		bool m_running = true;
		float m_lastFrameTime = 0.0f;

		std::unique_ptr<Window> m_window;
		std::unique_ptr<IRenderDevice> m_renderDevice;

		void initWindow();
		void initRender();
		void mainLoop();
		void cleanup();
	};
}

#endif