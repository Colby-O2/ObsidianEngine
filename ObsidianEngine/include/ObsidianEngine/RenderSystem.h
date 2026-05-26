#ifndef RENDER_SYSTEM_H_
#define RENDER_SYSTEM_H_

#include "ECS/System.h"
#include "MeshComponent.h"
#include "VulkanDevice.h"

#include <iostream>
#include <vector>

#include <iostream>

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
                    mesh.gpuId = m_device->uploadMeshData(mesh.vertices, mesh.indices, mesh.isStatic);

                    //mesh.vertices.clear(); 
                    //mesh.indices.clear();
                }
                else if (mesh.isDirty)
                {
                    if (!mesh.isStatic)
                    {
                        m_device->updateDynamicMesh(mesh.gpuId, mesh.vertices, mesh.indices);
                        mesh.isDirty = false;
                    }
                    else 
                    {
                        std::cout << "A static mesh was set Dirty but you can't modify a static mesh!" << std::endl;
                    }

                    //mesh.vertices.clear();
                    //mesh.indices.clear();
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