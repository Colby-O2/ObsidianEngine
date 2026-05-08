#include "VulkanSwapchain.h"

#include <iostream>

namespace ObsidianEngine
{
	VulkanSwapchain::VulkanSwapchain(const vk::raii::Device& device, const vk::raii::PhysicalDevice& physicalDevice, const vk::raii::SurfaceKHR& surface, Window* window) : 
		m_device(device),
		m_physicalDevice(physicalDevice),
		m_surface(surface),
		m_window(window)
	{
		createSwapchain(device, physicalDevice, surface, window);
		createImageView(device);
	}

	VulkanSwapchain::~VulkanSwapchain()
	{
		cleanupSwapchain();
	}

	void VulkanSwapchain::createSwapchain(const vk::raii::Device& device, const vk::raii::PhysicalDevice& physicalDevice, const vk::raii::SurfaceKHR& surface, Window* window)
	{
		auto surfaceCapabilities = physicalDevice.getSurfaceCapabilitiesKHR(*surface);
		m_extent = chooseSwapExtents(surfaceCapabilities, window);
		uint32_t minImageCount = chooseSwapMinImageCount(surfaceCapabilities);

		std::vector<vk::SurfaceFormatKHR> availableFormats = physicalDevice.getSurfaceFormatsKHR(*surface);
		m_format = chooseSwapSurfaceFormat(availableFormats);

		auto availablePresentModes = physicalDevice.getSurfacePresentModesKHR(*surface);
		vk::PresentModeKHR presentMode = chooseSwapPresentMode(availablePresentModes);

		vk::SwapchainCreateInfoKHR swapChainCreateInfo
		{
			.surface = *surface,
			.minImageCount = minImageCount,
			.imageFormat = m_format.format,
			.imageColorSpace = m_format.colorSpace,
			.imageExtent = m_extent,
			.imageArrayLayers = 1,
			.imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
			.imageSharingMode = vk::SharingMode::eExclusive,
			.preTransform = surfaceCapabilities.currentTransform,
			.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
			.presentMode = presentMode,
			.clipped = true
		};

		m_swapchain = vk::raii::SwapchainKHR(device, swapChainCreateInfo);
		m_images = m_swapchain.getImages();
	}

	void VulkanSwapchain::createImageView(const vk::raii::Device& device)
	{
		assert(m_views.empty());

		vk::ImageViewCreateInfo imageViewCreateInfo
		{
			.viewType = vk::ImageViewType::e2D,
			.format = m_format.format,
			.subresourceRange = { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 }
		};

		imageViewCreateInfo.components =
		{
			vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity
		};

		for (auto& image : m_images)
		{
			imageViewCreateInfo.image = image;

			auto view = vk::raii::ImageView(device, imageViewCreateInfo);

			m_views.emplace_back(
				std::make_unique<VulkanTextureView>(std::move(view))
			);
		}
	}

	std::pair<vk::Result, uint32_t> VulkanSwapchain::acquireNextImage(vk::raii::Semaphore& presentCompleteSemaphore)
	{
		auto [result, index] = m_swapchain.acquireNextImage(UINT64_MAX, *presentCompleteSemaphore, nullptr);

		return std::pair<vk::Result, uint32_t>(result, index);
	}

	vk::Result VulkanSwapchain::present(vk::raii::Queue& graphicsQueue, vk::raii::Semaphore& renderFinishedSemaphore, uint32_t& imageIndex)
	{
		vk::PresentInfoKHR presentInfo{
			.waitSemaphoreCount = 1,
			.pWaitSemaphores = &*renderFinishedSemaphore,
			.swapchainCount = 1,
			.pSwapchains = &*m_swapchain,
			.pImageIndices = &imageIndex
		};

		try 
		{
			return graphicsQueue.presentKHR(presentInfo);
		}
		catch (const vk::OutOfDateKHRError& e) 
		{
			return vk::Result::eErrorOutOfDateKHR;
		}
		catch (const vk::SystemError& e) 
		{
			return static_cast<vk::Result>(e.code().value());
		}
	}

	void VulkanSwapchain::transitionLayout(
		vk::raii::CommandBuffer& cmd,
		uint32_t                imageIndex,
		vk::ImageLayout         old_layout,
		vk::ImageLayout         new_layout,
		vk::AccessFlags2        src_access_mask,
		vk::AccessFlags2        dst_access_mask,
		vk::PipelineStageFlags2 src_stage_mask,
		vk::PipelineStageFlags2 dst_stage_mask)
	{
		vk::ImageMemoryBarrier2 barrier = {
			.srcStageMask = src_stage_mask,
			.srcAccessMask = src_access_mask,
			.dstStageMask = dst_stage_mask,
			.dstAccessMask = dst_access_mask,
			.oldLayout = old_layout,
			.newLayout = new_layout,
			.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.image = m_images[imageIndex],
			.subresourceRange = {
				   .aspectMask = vk::ImageAspectFlagBits::eColor,
				   .baseMipLevel = 0,
				   .levelCount = 1,
				   .baseArrayLayer = 0,
				   .layerCount = 1} };
		vk::DependencyInfo dependencyInfo = {
			.dependencyFlags = {},
			.imageMemoryBarrierCount = 1,
			.pImageMemoryBarriers = &barrier };
		cmd.pipelineBarrier2(dependencyInfo);
	}

	vk::SurfaceFormatKHR  VulkanSwapchain::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats)
	{
		const auto formatIt = std::ranges::find_if(availableFormats, [](const auto& format)
			{
				return format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear;
			}
		);

		return formatIt != availableFormats.end() ? *formatIt : availableFormats[0];
	}

	vk::PresentModeKHR  VulkanSwapchain::chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes)
	{
		assert(std::ranges::any_of(availablePresentModes, [](const vk::PresentModeKHR presentMode) { return presentMode == vk::PresentModeKHR::eFifo; }));
		return std::ranges::any_of(availablePresentModes, [](const vk::PresentModeKHR presentMode) { return presentMode == vk::PresentModeKHR::eMailbox; }) ? vk::PresentModeKHR::eMailbox : vk::PresentModeKHR::eFifo;
	}

	vk::Extent2D VulkanSwapchain::chooseSwapExtents(vk::SurfaceCapabilitiesKHR const& capabilities, Window* window)
	{
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		{
			return capabilities.currentExtent;
		}

		int width;
		int height;
		window->getFramebufferSize(&width, &height);

		return
		{
			std::clamp<uint32_t>(width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
			std::clamp<uint32_t>(height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height),
		};
	}

	uint32_t VulkanSwapchain::chooseSwapMinImageCount(const vk::SurfaceCapabilitiesKHR& capabilities)
	{
		auto minImageCount = std::max(3u, capabilities.minImageCount);
		if ((0 < capabilities.maxImageCount) && (capabilities.maxImageCount < minImageCount))
		{
			minImageCount = capabilities.maxImageCount;
		}

		return minImageCount;
	}

	void VulkanSwapchain::recreateSwapchain(const vk::raii::Device& device, const vk::raii::PhysicalDevice& physicalDevice, const vk::raii::SurfaceKHR& surface)
	{
		int width = 0, height = 0;
		m_window->getFramebufferSize(&width, &height);
		while (width == 0 || height == 0) 
		{
			m_window->getFramebufferSize(&width, &height);
			m_window->waitEvents();
		}

		device.waitIdle();

		cleanupSwapchain();

		createSwapchain(device, physicalDevice, surface, m_window);
		createImageView(device);
	}

	void VulkanSwapchain::cleanupSwapchain()
	{
		m_views.clear();
		m_swapchain = nullptr;
	}
}
