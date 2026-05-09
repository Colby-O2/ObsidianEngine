#ifndef VERTEX_H_
#define VERTEX_H_

#include <vulkan/vulkan_raii.hpp>
#include <vector>

#include "Vector.h"

namespace ObsidianEngine
{
    struct Vertex
    {
        Vector2 pos;
        Vector3 color;

        Vertex() = default;
        Vertex(Vector2 _pos, Vector3 _color) : pos(_pos), color(_color) {}

        static vk::VertexInputBindingDescription getBindingDescription()
        {
            return { .binding = 0, .stride = sizeof(Vertex), .inputRate = vk::VertexInputRate::eVertex };
        }

        static std::array<vk::VertexInputAttributeDescription, 2> getAttributeDescriptions()
        {
            return {
                {
                    {.location = 0, .binding = 0, .format = vk::Format::eR32G32Sfloat, .offset = offsetof(Vertex, pos)},
                    {.location = 1, .binding = 0, .format = vk::Format::eR32G32B32Sfloat, .offset = offsetof(Vertex, color)}
                }
            };
        }
    };
}

#endif