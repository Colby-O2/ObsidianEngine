#ifndef SIERPINSKI_TRIANGLE_APPLICATION_H_
#define SIERPINSKI_TRIANGLE_APPLICATION_H_

#include "ObsidianEngine/Application.h"
#include "ObsidianEngine/Vertex.h"
#include "ObsidianEngine/MeshComponent.h"
#include "ObsidianEngine/Entity.h"

#include <vector>
#include <unordered_map>

using namespace ObsidianEngine;

class SpinningTriangleApplication : public ObsidianEngine::Application
{
protected:
    void onStartup() override 
    {
        std::cout << "Yo!" << std::endl;

        const std::vector<Vertex> vertices = {
            {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
            {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
            {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
        };

        const std::vector<uint16_t> indices = {
            0, 1, 2, 2, 3, 0
        };

        EntityID entity = m_activeScene->createEntity();
        m_activeScene->getRegistry().addComponent<MeshComponent>(entity, vertices, indices, -1, false);
    }
};

#endif