#ifndef MESH_RENDER_DATA_H_
#define MESH_RENDER_DATA_H_

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan_raii.hpp>

#include <cinttypes>

namespace ObsidianEngine
{
    struct MeshRenderData 
    {
        vk::Buffer vertexBuffer;
        vk::Buffer indexBuffer;
        uint32_t indexCount;
    };
}

#endif
