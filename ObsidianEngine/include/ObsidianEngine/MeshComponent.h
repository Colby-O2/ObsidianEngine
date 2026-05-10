#ifndef MESH_COMPONENT_H_
#define MESH_COMPONENT_H_

#include "Vertex.h"
#include "MeshRenderData.h"

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan_raii.hpp>

#include <vector>
#include <cinttypes>

namespace ObsidianEngine
{
    struct MeshComponent {
        std::vector<Vertex> vertices;
        std::vector<uint16_t> indices;
        uint32_t gpuId = -1;
        bool isStatic = false;
        bool isDirty = false;

        MeshComponent() = default;

        MeshComponent(
            std::vector<Vertex> verts,
            std::vector<uint16_t> inds,
            uint32_t id = -1,
            bool isStatic = false
        )
            : vertices(std::move(verts))
            , indices(std::move(inds))
            , gpuId(id)
            , isStatic(isStatic)
        {
        }
    };
}

#endif