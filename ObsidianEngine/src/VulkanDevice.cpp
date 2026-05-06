#include "../include/VulkanDevice.h"
#include "../include/Window.h"

#include "../include/ShaderCompiler.h"

#include <algorithm>
#include "../include/VulkanPipelineBuilder.h"

namespace ObsidianEngine
{
	void VulkanDevice::init(Window* window)
	{
		createInstance();

		setupDebugMessenger();

		createSurface(window);
		pickDevice();
		createLogicalDevice();
		createSwapChain(window);

		createGraphicsPipeline();
		createCommandPool();
		createCommandBuffers();

		createSyncObjects();

	}

	void VulkanDevice::beginFrame() 
	{

	}

	void VulkanDevice::endFrame()
	{

	}

	void VulkanDevice::drawTriangle()
	{
		drawFrame();
	}

	void VulkanDevice::waitIdle()
	{
		m_device.waitIdle();
	}

	std::vector<const char*> VulkanDevice::getRequiredInstanceExtension()
	{
		uint32_t glfwExtensionCount = 0;
		auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		if (ENABLE_VAILDATION_LAYERS)
		{
			extensions.push_back(vk::EXTDebugUtilsExtensionName);
		}

		return extensions;
	}

	void VulkanDevice::createInstance()
	{
		constexpr vk::ApplicationInfo appInfo
		{
		.pApplicationName = "Obsidian",
		.applicationVersion = VK_MAKE_VERSION(1, 0, 0),
		.pEngineName = "Obsidian Engine",
		.engineVersion = VK_MAKE_VERSION(1, 0, 0),
		.apiVersion = vk::ApiVersion14
		};

		// Vaildation Layer setup
		std::vector<char const*> requiredLayers;

		if (ENABLE_VAILDATION_LAYERS)
		{
			requiredLayers.assign(s_vaildLayers.begin(), s_vaildLayers.end());
		}

		auto layerProperties = m_context.enumerateInstanceLayerProperties();
		auto unsupportedLayerIt = std::ranges::find_if(requiredLayers,
			[&layerProperties](auto const& requiredLayer)
			{
				return std::ranges::none_of(layerProperties,
					[requiredLayer](auto const& layerProperty)
					{
						return strcmp(layerProperty.layerName, requiredLayer) == 0;
					}
				);
			}
		);

		if (unsupportedLayerIt != requiredLayers.end())
		{
			throw std::runtime_error("Required layer not supported: " + std::string(*unsupportedLayerIt));
		}

		// GLFW Window Extension Setup
		auto requiredExtensions = getRequiredInstanceExtension();

		auto extensionProperties = m_context.enumerateInstanceExtensionProperties();
		auto unsupportedPropertyIt = std::ranges::find_if(
			requiredExtensions,
			[&extensionProperties](auto const& requiredExtension)
			{
				return std::ranges::none_of(extensionProperties,
					[requiredExtension](auto const& extensionProperty)
					{
						return strcmp(extensionProperty.extensionName, requiredExtension) == 0;
					}
				);
			}
		);

		if (unsupportedPropertyIt != requiredExtensions.end())
		{
			throw std::runtime_error("Required extension not supported: " + std::string(*unsupportedLayerIt));
		}

		// Create VK Instance
		vk::InstanceCreateInfo createInfo
		{
			.pApplicationInfo = &appInfo,
			.enabledLayerCount = static_cast<uint32_t>(requiredLayers.size()),
			.ppEnabledLayerNames = requiredLayers.data(),
			.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size()),
			.ppEnabledExtensionNames = requiredExtensions.data()
		};

		m_instance = vk::raii::Instance(m_context, createInfo);
	}

	void VulkanDevice::setupDebugMessenger()
	{
		if (!ENABLE_VAILDATION_LAYERS) return;

		vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);
		vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);

		vk::DebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfoEXT{
			.messageSeverity = severityFlags,
			.messageType = messageTypeFlags,
			.pfnUserCallback = &debugCallback
		};

		m_debugMessenger = m_instance.createDebugUtilsMessengerEXT(debugUtilsMessengerCreateInfoEXT);
	}

	void VulkanDevice::createSurface(Window* window)
	{
		VkSurfaceKHR surface;
		if (glfwCreateWindowSurface(*m_instance, window->getNativeWindow(), nullptr, &surface))
		{
			throw std::runtime_error("Failed to create window surface!");
		}

		m_surface = vk::raii::SurfaceKHR(m_instance, surface);
	}

	bool VulkanDevice::isDeviceSuitable(vk::raii::PhysicalDevice const& physicalDevice)
	{
		auto deviceProperties = physicalDevice.getProperties();
		auto deviceFeatures = physicalDevice.getFeatures();
		auto queueFamilies = physicalDevice.getQueueFamilyProperties();

		bool supportsVulkan1_3 = deviceProperties.apiVersion >= vk::ApiVersion13;
		bool supportsGraphics = std::ranges::any_of(queueFamilies, [](auto const& qfp)
			{
				return !!(qfp.queueFlags & vk::QueueFlagBits::eGraphics);
			}
		);

		auto availableDeviceExtensions = physicalDevice.enumerateDeviceExtensionProperties();
		bool supportsAllRequiredExtensions = std::ranges::all_of(s_requiredDeviceExtensions, [&availableDeviceExtensions](auto const& requiredDeviceExtension)
			{
				return std::ranges::any_of(availableDeviceExtensions, [requiredDeviceExtension](auto const& availableDeviceExtension)
					{
						return strcmp(availableDeviceExtension.extensionName, requiredDeviceExtension) == 0;
					}
				);
			}
		);

		auto features = physicalDevice.template getFeatures2<vk::PhysicalDeviceFeatures2,
			vk::PhysicalDeviceVulkan11Features,
			vk::PhysicalDeviceVulkan13Features,
			vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>();
		bool supportsRequiredFeatures = features.template get<vk::PhysicalDeviceVulkan11Features>().shaderDrawParameters &&
			features.template get<vk::PhysicalDeviceVulkan13Features>().dynamicRendering &&
			features.template get<vk::PhysicalDeviceVulkan13Features>().synchronization2 &&
			features.template get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>().extendedDynamicState;

		return supportsVulkan1_3 && supportsGraphics && supportsAllRequiredExtensions && supportsRequiredFeatures;
	}

	void VulkanDevice::pickDevice()
	{
		std::vector<vk::raii::PhysicalDevice> physicalDevices = m_instance.enumeratePhysicalDevices();

		auto const devIter = std::ranges::find_if(physicalDevices, [&](auto const& physicalDevice) { return isDeviceSuitable(physicalDevice); });
		if (devIter == physicalDevices.end())
		{
			throw std::runtime_error("Failed to find a suitable GPU!");
		}

		m_physicalDevice = *devIter;
	}

	void VulkanDevice::createLogicalDevice()
	{
		std::vector<vk::QueueFamilyProperties> queueFamilyProperties = m_physicalDevice.getQueueFamilyProperties();

		m_queueIndex = ~0;
		for (uint32_t qfpIndex = 0; qfpIndex < queueFamilyProperties.size(); qfpIndex++)
		{
			if (queueFamilyProperties[qfpIndex].queueFlags & vk::QueueFlagBits::eGraphics && m_physicalDevice.getSurfaceSupportKHR(qfpIndex, *m_surface))
			{
				m_queueIndex = qfpIndex;
				break;
			}
		}

		if (m_queueIndex == ~0)
		{
			throw std::runtime_error("Could not find a queue for graphics and presentation!");
		}

		vk::StructureChain<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan11Features, vk::PhysicalDeviceVulkan13Features, vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT> featureChain = {
			{},
			{.shaderDrawParameters = true },
			{.synchronization2 = true, .dynamicRendering = true},
			{.extendedDynamicState = true}
		};

		float queuePriority = 0.5f;
		vk::DeviceQueueCreateInfo deviceQueueCreateInfo
		{
			.queueFamilyIndex = m_queueIndex,
			.queueCount = 1,
			.pQueuePriorities = &queuePriority
		};

		vk::DeviceCreateInfo deviceCreateInfo
		{
			.pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>(),
			.queueCreateInfoCount = 1,
			.pQueueCreateInfos = &deviceQueueCreateInfo,
			.enabledExtensionCount = static_cast<uint32_t>(s_requiredDeviceExtensions.size()),
			.ppEnabledExtensionNames = s_requiredDeviceExtensions.data()
		};

		m_device = vk::raii::Device(m_physicalDevice, deviceCreateInfo);
		m_graphicsQueue = vk::raii::Queue(m_device, m_queueIndex, 0);
	}

	void VulkanDevice::createSwapChain(Window* window)
	{
		m_swapchain = std::make_unique<VulkanSwapchain>(m_device, m_physicalDevice, m_surface, window);
	}

	void VulkanDevice::createGraphicsPipeline()
	{
		auto compiler = ShaderCompiler();
		auto shaderBlob = compiler.compile("C:/Users/okeef/source/repos/ObsidianEngine/ObsidianEngine/shaders/shader.slang", ShaderBackend::SPIRV);

		const char* data = static_cast<const char*>(shaderBlob.get()->getBufferPointer());
		size_t length = shaderBlob.get()->getBufferSize();

		std::vector<char> shaderData(data, data + length);

		vk::raii::ShaderModule shaderModule = createShaderModule(shaderData);

		vk::PipelineLayoutCreateInfo pipelineLayoutInfo{ .setLayoutCount = 0, .pushConstantRangeCount = 0 };
		m_pipelineLayout = vk::raii::PipelineLayout(m_device, pipelineLayoutInfo);

		VulkanPipelineBuilder builder(m_device);

		VulkanPipeline pipeline = builder.setShaders(shaderModule, shaderModule)
			.setTopology(vk::PrimitiveTopology::eTriangleList)
			.setRasterizer(vk::PolygonMode::eFill, vk::CullModeFlagBits::eBack, vk::FrontFace::eClockwise)
			.setColorBlend(false)
			.setDepthStencil(true, true, vk::CompareOp::eLess)
			.build(m_pipelineLayout, m_swapchain->getFormat()->format, vk::Format::eD32Sfloat);
		m_pipeline = std::make_unique<VulkanPipeline>(std::move(pipeline));
	}

	void VulkanDevice::createCommandPool()
	{
		vk::CommandPoolCreateInfo poolInfo{ .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
										   .queueFamilyIndex = m_queueIndex };
		m_commandPool = vk::raii::CommandPool(m_device, poolInfo);
	}

	void VulkanDevice::createCommandBuffers()
	{
		m_commandBuffers.clear();
		vk::CommandBufferAllocateInfo allocInfo{ .commandPool = m_commandPool, .level = vk::CommandBufferLevel::ePrimary, .commandBufferCount = MAX_FRAMES_IN_FLIGHT };
		m_commandBuffers = vk::raii::CommandBuffers(m_device, allocInfo);
	}
	void VulkanDevice::createSyncObjects()
	{
		assert(m_presentCompleteSemaphores.empty() && m_renderFinishedSemaphores.empty() && m_inFlightFences.empty());

		for (size_t i = 0; i < m_swapchain->getImageCount(); i++)
		{
			m_renderFinishedSemaphores.emplace_back(m_device, vk::SemaphoreCreateInfo());
		}

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			m_presentCompleteSemaphores.emplace_back(m_device, vk::SemaphoreCreateInfo());
			m_inFlightFences.emplace_back(m_device, vk::FenceCreateInfo{ .flags = vk::FenceCreateFlagBits::eSignaled });
		}
	}

	void VulkanDevice::drawFrame()
	{
		auto fenceResult = m_device.waitForFences(*m_inFlightFences[m_frameIndex], vk::True, UINT64_MAX);
		if (fenceResult != vk::Result::eSuccess)
		{
			throw std::runtime_error("Failed to wait for fence!");
		}

		auto [result, imageIndex] = m_swapchain->acquireNextImage(m_presentCompleteSemaphores[m_frameIndex]);


		if (result == vk::Result::eErrorOutOfDateKHR)
		{
			m_swapchain->recreateSwapchain(m_device, m_physicalDevice, m_surface);
			return;
		}

		if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR)
		{
			assert(result == vk::Result::eTimeout || result == vk::Result::eNotReady);
			throw std::runtime_error("Failed to acquire swap chain image!");
		}

		m_device.resetFences(*m_inFlightFences[m_frameIndex]);

		m_commandBuffers[m_frameIndex].reset();
		recordCommandBuffer(imageIndex);

		vk::PipelineStageFlags waitDestinationStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
		const vk::SubmitInfo   submitInfo{ .waitSemaphoreCount = 1,
										  .pWaitSemaphores = &*m_presentCompleteSemaphores[m_frameIndex],
										  .pWaitDstStageMask = &waitDestinationStageMask,
										  .commandBufferCount = 1,
										  .pCommandBuffers = &*m_commandBuffers[m_frameIndex],
										  .signalSemaphoreCount = 1,
										  .pSignalSemaphores = &*m_renderFinishedSemaphores[imageIndex] };
		m_graphicsQueue.submit(submitInfo, *m_inFlightFences[m_frameIndex]);

		result = m_swapchain->present(m_graphicsQueue, m_renderFinishedSemaphores[imageIndex], imageIndex);

		if ((result == vk::Result::eSuboptimalKHR) || (result == vk::Result::eErrorOutOfDateKHR) || m_framebufferResized)
		{
			m_framebufferResized = false;
			m_swapchain->recreateSwapchain(m_device, m_physicalDevice, m_surface);
		}
		else
		{
			assert(result == vk::Result::eSuccess);
		}

		m_frameIndex = (m_frameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
	}

	void VulkanDevice::recordCommandBuffer(uint32_t imageIndex)
	{
		auto& commandBuffer = m_commandBuffers[m_frameIndex];
		commandBuffer.begin({});

		m_swapchain->transitionLayout(
			commandBuffer,
			imageIndex,
			vk::ImageLayout::eUndefined,
			vk::ImageLayout::eColorAttachmentOptimal,
			{},
			vk::AccessFlagBits2::eColorAttachmentWrite,
			vk::PipelineStageFlagBits2::eColorAttachmentOutput,
			vk::PipelineStageFlagBits2::eColorAttachmentOutput
		);

		vk::ClearValue clearColor = vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f);
		vk::RenderingAttachmentInfo attachmentInfo = {
				.imageView = static_cast<VulkanTextureView*>(m_swapchain->getView(imageIndex))->get(),
				.imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
				.loadOp = vk::AttachmentLoadOp::eClear,
				.storeOp = vk::AttachmentStoreOp::eStore,
				.clearValue = clearColor };

		vk::RenderingInfo renderingInfo = {
				.renderArea = {.offset = {0, 0}, .extent = m_swapchain->getExtent()},
				.layerCount = 1,
				.colorAttachmentCount = 1,
				.pColorAttachments = &attachmentInfo };

		commandBuffer.beginRendering(renderingInfo);

	    m_pipeline->bind(commandBuffer, vk::PipelineBindPoint::eGraphics);

		vk::Viewport viewport{ 0.0f, 0.0f, (float)m_swapchain->getExtent().width, (float)m_swapchain->getExtent().height, 0.0f, 1.0f };
		vk::Rect2D scissor{ {0, 0}, m_swapchain->getExtent() };
		commandBuffer.setViewport(0, viewport);
		commandBuffer.setScissor(0, scissor);

		commandBuffer.draw(3, 1, 0, 0);

		commandBuffer.endRendering();

		m_swapchain->transitionLayout(
			commandBuffer,
			imageIndex,
			vk::ImageLayout::eColorAttachmentOptimal,
			vk::ImageLayout::ePresentSrcKHR,
			vk::AccessFlagBits2::eColorAttachmentWrite,
			{},
			vk::PipelineStageFlagBits2::eColorAttachmentOutput,
			vk::PipelineStageFlagBits2::eBottomOfPipe
		);

		commandBuffer.end();
	}
}