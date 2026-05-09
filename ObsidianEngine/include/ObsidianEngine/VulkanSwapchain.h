#ifndef VULKAN_SWAPCHAIN_H_
#define VULKAN_SWAPCHAIN_H_

#include "Window.h"

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan_raii.hpp>

#include <cstdint>

namespace ObsidianEngine
{
    class ITextureView {
    public:
        virtual ~ITextureView() = default;
    };

	class VulkanTextureView : public ITextureView {
	public:
		VulkanTextureView(vk::raii::ImageView view) : m_view(std::move(view)) {}
		const vk::raii::ImageView& get() const { return m_view; }
	private:
		vk::raii::ImageView m_view;
	};

    class VulkanSwapchain {
    public:
        VulkanSwapchain(const vk::raii::Device& device, const vk::raii::PhysicalDevice& physicalDevice, const vk::raii::SurfaceKHR& surface, Window* window);
        ~VulkanSwapchain();

        VulkanSwapchain(const VulkanSwapchain&) = delete;
        VulkanSwapchain& operator=(const VulkanSwapchain&) = delete;

        std::pair<vk::Result, uint32_t> acquireNextImage(vk::raii::Semaphore& presentCompleteSemaphore);
        vk::Result present(vk::raii::Queue& graphicsQueue, vk::raii::Semaphore& renderFinishedSemaphore, uint32_t& imageIndex);

        void transitionLayout(vk::raii::CommandBuffer& cmd, uint32_t imageIndex, vk::ImageLayout old_layout, vk::ImageLayout new_layout, vk::AccessFlags2 src_access_mask, vk::AccessFlags2 dst_access_mask, vk::PipelineStageFlags2 src_stage_mask, vk::PipelineStageFlags2 dst_stage_mask);

        void recreateSwapchain(const vk::raii::Device& device, const vk::raii::PhysicalDevice& physicalDevice, const vk::raii::SurfaceKHR& surface);

        size_t getImageCount() const { return m_images.size(); }
        vk::Extent2D  getExtent() const { return m_extent; }
        ITextureView* getView(uint32_t index) const { return m_views[index].get(); }
        const vk::SurfaceFormatKHR* getFormat() const { return &m_format; }
        const vk::raii::SwapchainKHR& get() const { return m_swapchain; }

    private:
        const vk::raii::Device& m_device;
        const vk::raii::PhysicalDevice& m_physicalDevice;
        const vk::raii::SurfaceKHR& m_surface;
        Window* m_window;

        vk::raii::SwapchainKHR m_swapchain = nullptr;
        std::vector<vk::Image> m_images;
        std::vector<std::unique_ptr<VulkanTextureView>> m_views;
        vk::Extent2D  m_extent;
        vk::SurfaceFormatKHR m_format;

        void createSwapchain(const vk::raii::Device& device, const vk::raii::PhysicalDevice& physicalDevice, const vk::raii::SurfaceKHR& surface, Window* window);
        void createImageView(const vk::raii::Device& device);
        void cleanupSwapchain();

        vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> & availableFormats);
        vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);
        vk::Extent2D chooseSwapExtents(const vk::SurfaceCapabilitiesKHR& capabilities, Window* window);
        uint32_t chooseSwapMinImageCount(const vk::SurfaceCapabilitiesKHR& capabilities);
    };
}

#endif