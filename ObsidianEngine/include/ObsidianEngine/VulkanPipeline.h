#ifndef VULKAN_PIPELINE_H_
#define VULKAN_PIPELINE_H_

#include <vulkan/vulkan_raii.hpp>

namespace ObsidianEngine
{
	class VulkanPipeline
	{
	public:
		VulkanPipeline(vk::raii::Pipeline&& pipeline, vk::raii::PipelineLayout&& layout)
			: m_pipeline(std::move(pipeline)), 
			  m_layout(std::move(layout)) {}
		~VulkanPipeline() = default;

		VulkanPipeline(VulkanPipeline&& other) = default;
		VulkanPipeline& operator=(VulkanPipeline&& other) = default;

		VulkanPipeline(const VulkanPipeline&) = delete;
		VulkanPipeline& operator=(const VulkanPipeline&) = delete;

		void bind(vk::raii::CommandBuffer& cb, vk::PipelineBindPoint bindPoint = vk::PipelineBindPoint::eGraphics) const {
			cb.bindPipeline(bindPoint, *m_pipeline);
		}

		const vk::raii::PipelineLayout& getLayout() const { return m_layout; }

	private:
		vk::raii::PipelineLayout m_layout = nullptr;
		vk::raii::Pipeline       m_pipeline = nullptr;
	};
}

#endif