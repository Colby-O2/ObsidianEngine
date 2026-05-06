#ifndef VULKAN_PIPELINE_BUILDER_H_
#define VULKAN_PIPELINE_BUILDER_H_

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan_raii.hpp>
#include <vector>

namespace ObsidianEngine
{
    class VulkanPipeline;

	class VulkanPipelineBuilder
	{
	public:
		VulkanPipelineBuilder(const vk::raii::Device& device);

        VulkanPipelineBuilder& setShaders(vk::raii::ShaderModule& vert, vk::raii::ShaderModule& frag);
        VulkanPipelineBuilder& setTopology(vk::PrimitiveTopology topology);
        VulkanPipelineBuilder& setRasterizer(vk::PolygonMode polyMode, vk::CullModeFlags cullMode, vk::FrontFace frontFace = vk::FrontFace::eClockwise);
        VulkanPipelineBuilder& setMultisampling(vk::SampleCountFlagBits samples);
        VulkanPipelineBuilder& setColorBlend(bool enable);
        VulkanPipelineBuilder& setDepthStencil(bool depthTest, bool depthWrite, vk::CompareOp compareOp);

        VulkanPipeline build(vk::raii::PipelineLayout& layout, vk::Format colorFormat, vk::Format depthFormat = vk::Format::eUndefined);

    private:
        const vk::raii::Device& m_device;

        std::vector<vk::PipelineShaderStageCreateInfo> m_shaderStages;
        vk::PipelineInputAssemblyStateCreateInfo m_inputAssembly{};
        vk::PipelineRasterizationStateCreateInfo m_rasterizer{};
        vk::PipelineMultisampleStateCreateInfo m_multisampling{};
        vk::PipelineColorBlendAttachmentState m_colorBlendAttachment{};
        vk::PipelineDepthStencilStateCreateInfo m_depthStencil{};
	};
}

#endif