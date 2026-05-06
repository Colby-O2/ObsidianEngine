#ifndef APPLICATION_H_
#define APPLICATION_H_

#include "../include/Window.h"
#include "../include/RenderDevice.h"

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include<array>

namespace ObsidianEngine
{
#define WIDTH 800
#define HEIGHT 600

	class Application {
	public:
		void run();
		void setFrameBufferRsizedFlag();

	private:

		std::unique_ptr<Window> m_window;
		std::unique_ptr<IRenderDevice> m_renderDevice;

		void initWindow();
		void initRender();
		void mainLoop();
		void cleanup();
	};
}

#endif