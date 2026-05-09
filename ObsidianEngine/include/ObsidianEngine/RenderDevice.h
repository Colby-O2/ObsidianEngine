#ifndef RENDER_DEVICE_H_
#define RENDER_DEVICE_H_

#include "MeshRenderData.h"

#include <span>

namespace ObsidianEngine
{
	class Window;

	class IRenderDevice
	{
	public:
		virtual ~IRenderDevice() = default;

		virtual void init(Window* window) = 0;
		virtual void beginFrame() = 0;
		virtual void endFrame() = 0;
		virtual void draw(std::span<MeshRenderData> renderList) = 0;
		virtual void waitIdle() = 0;

		bool m_framebufferResized = false;
	};
}

#endif