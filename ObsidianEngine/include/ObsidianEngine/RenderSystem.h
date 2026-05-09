#ifndef RENDER_SYSTEM_H_
#define RENDER_SYSTEM_H_

#include "System.h"
#include "MeshComponent.h"
#include "VulkanDevice.h"

#include <iostream>
#include <vector>

namespace ObsidianEngine
{
	class RenderSystem : public ISystem
	{
    public:
        RenderSystem(VulkanDevice* device) : m_device(device) {}
        ~RenderSystem() override = default;

        void update(Registry& registry, float dt) override
        {
            m_device->beginFrame();

            m_renderList.clear();

            auto view = registry.view<MeshComponent>();

            for (auto entity : view) 
            {
                auto& mesh = registry.getComponent<MeshComponent>(entity);

                if (mesh.gpuId == -1) 
                {
                    mesh.gpuId = m_device->uploadMeshData(mesh.vertices, mesh.indices);

                    mesh.vertices.clear(); 
                    mesh.indices.clear();
                }
                else if (mesh.isDirty)
                {
                    m_device->updateDynamicMesh(mesh.gpuId, mesh.vertices, mesh.indices);
                    mesh.isDirty = false;
                }

                m_renderList.push_back(m_device->getGPUBufferData(mesh.gpuId));
            }

            m_device->draw(m_renderList);
            m_device->endFrame();
        }

    private:
        VulkanDevice* m_device;
        std::vector<MeshRenderData> m_renderList;
	};

}

#endif