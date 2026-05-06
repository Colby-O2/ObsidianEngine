#ifndef VULKAN_DEVICE_H_
#define VULKAN_DEVICE_H_

#include "../include/RenderDevice.h"
#include "../include/VulkanSwapchain.h"
#include "../include/VulkanPipeline.h"

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan_raii.hpp>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include<array>

namespace ObsidianEngine
{
    static const std::array s_vaildLayers =
    {
        "VK_LAYER_KHRONOS_validation"
    };

    static const std::array s_requiredDeviceExtensions =
    {
        vk::KHRSwapchainExtensionName
    };

    int constexpr MAX_FRAMES_IN_FLIGHT = 2;

#ifdef NDEBUG
#define ENABLE_VAILDATION_LAYERS false
#else
#define ENABLE_VAILDATION_LAYERS true
#endif


    class VulkanDevice : public IRenderDevice {
    public:
        VulkanDevice() = default;

        void init(Window* window) override;
        void beginFrame() override;
        void endFrame() override;
        void drawTriangle() override;
        void waitIdle() override;

    private:
        vk::raii::Context                    m_context;
        vk::raii::Instance                   m_instance = nullptr;
        vk::raii::PhysicalDevice             m_physicalDevice = nullptr;
        vk::raii::Device                     m_device = nullptr;
        vk::raii::Queue                      m_graphicsQueue = nullptr;
        vk::raii::DebugUtilsMessengerEXT     m_debugMessenger = nullptr;
        vk::raii::SurfaceKHR                 m_surface = nullptr;

        std::unique_ptr<VulkanSwapchain>     m_swapchain;

        std::unique_ptr<VulkanPipeline>      m_pipeline;
        vk::raii::PipelineLayout             m_pipelineLayout = nullptr;
        vk::raii::CommandPool                m_commandPool = nullptr;
        std::vector<vk::raii::CommandBuffer> m_commandBuffers;

        std::vector<vk::raii::Semaphore>     m_presentCompleteSemaphores;
        std::vector<vk::raii::Semaphore>     m_renderFinishedSemaphores;
        std::vector<vk::raii::Fence>         m_inFlightFences;
        uint32_t                             m_frameIndex = 0;


        uint32_t                            m_currentImageIndex = 0;
        uint32_t                            m_queueIndex = ~0;

        void createInstance();
        void setupDebugMessenger();
        void createSurface(Window* window);
        void pickDevice();
        void createLogicalDevice();
        void createSwapChain(Window* window);
        void createGraphicsPipeline();
        void createSyncObjects();
        void createCommandPool();
        void createCommandBuffers();

        void recordCommandBuffer(uint32_t imageIndex);

        void drawFrame();

        std::vector<const char*> getRequiredInstanceExtension();
        bool isDeviceSuitable(vk::raii::PhysicalDevice const& pd);

        static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity, vk::DebugUtilsMessageTypeFlagsEXT type, const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
        {
            std::cerr << "Validation layer: type " << to_string(type) << " msg: " << pCallbackData->pMessage << std::endl;

            return vk::False;
        }

        [[nodiscard]] vk::raii::ShaderModule createShaderModule(const std::vector<char>& code) const
        {
            vk::ShaderModuleCreateInfo createInfo{ .codeSize = code.size() * sizeof(char), .pCode = reinterpret_cast<const uint32_t*>(code.data()) };
            vk::raii::ShaderModule shaderModule{ m_device, createInfo };
            return shaderModule;
        }
    };
}

#endif 