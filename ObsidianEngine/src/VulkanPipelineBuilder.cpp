#include "../include/VulkanPipelineBuilder.h"
#include "../include/VulkanPipeline.h"

namespace ObsidianEngine
{
    VulkanPipelineBuilder::VulkanPipelineBuilder(const vk::raii::Device& device)
        : m_device(device)
    {
        m_inputAssembly.topology = vk::PrimitiveTopology::eTriangleList;
        m_inputAssembly.primitiveRestartEnable = vk::False;

        m_rasterizer.depthClampEnable = vk::False;
        m_rasterizer.rasterizerDiscardEnable = vk::False;
        m_rasterizer.polygonMode = vk::PolygonMode::eFill;
        m_rasterizer.lineWidth = 1.0f;
        m_rasterizer.cullMode = vk::CullModeFlagBits::eBack;
        m_rasterizer.frontFace = vk::FrontFace::eClockwise;
        m_rasterizer.depthBiasEnable = vk::False;

        m_multisampling.sampleShadingEnable = vk::False;
        m_multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;

        m_colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
            vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
        m_colorBlendAttachment.blendEnable = vk::False;

        m_depthStencil.depthTestEnable = vk::True;
        m_depthStencil.depthWriteEnable = vk::True;
        m_depthStencil.depthCompareOp = vk::CompareOp::eLess;
        m_depthStencil.depthBoundsTestEnable = vk::False;
        m_depthStencil.stencilTestEnable = vk::False;
    }

    VulkanPipelineBuilder& VulkanPipelineBuilder::setShaders(vk::raii::ShaderModule& vert, vk::raii::ShaderModule& frag)
    {
        m_shaderStages.clear();

        vk::PipelineShaderStageCreateInfo vertShaderStageInfo{ .stage = vk::ShaderStageFlagBits::eVertex, .module = vert, .pName = "vertMain" };
        vk::PipelineShaderStageCreateInfo fragShaderStageInfo{ .stage = vk::ShaderStageFlagBits::eFragment, .module = frag, .pName = "fragMain" };

        m_shaderStages.push_back(vertShaderStageInfo);
        m_shaderStages.push_back(fragShaderStageInfo);

        return *this;
    }

    VulkanPipelineBuilder& VulkanPipelineBuilder::setTopology(vk::PrimitiveTopology topology)
    {
        m_inputAssembly.topology = topology;
        return *this;
    }

    VulkanPipelineBuilder& VulkanPipelineBuilder::setRasterizer(vk::PolygonMode polyMode, vk::CullModeFlags cullMode, vk::FrontFace frontFace)
    {
        m_rasterizer.polygonMode = polyMode;
        m_rasterizer.cullMode = cullMode;
        m_rasterizer.frontFace = frontFace;
        m_rasterizer.lineWidth = 1.0f;
        return *this;
    }

    VulkanPipelineBuilder& VulkanPipelineBuilder::setMultisampling(vk::SampleCountFlagBits samples)
    {
        m_multisampling.rasterizationSamples = samples;
        return *this;
    }

    VulkanPipelineBuilder& VulkanPipelineBuilder::setColorBlend(bool enable)
    {
        m_colorBlendAttachment.blendEnable = enable ? vk::True : vk::False;
        return *this;
    }

    VulkanPipelineBuilder& VulkanPipelineBuilder::setDepthStencil(bool depthTest, bool depthWrite, vk::CompareOp compareOp)
    {
        m_depthStencil.depthTestEnable = depthTest;
        m_depthStencil.depthWriteEnable = depthWrite;
        m_depthStencil.depthCompareOp = compareOp;
        return *this;
    }

    VulkanPipeline VulkanPipelineBuilder::build(vk::raii::PipelineLayout& layout, vk::Format colorFormat, vk::Format depthFormat)
    {
        vk::PipelineViewportStateCreateInfo viewportState
        {
        .viewportCount = 1,
        .pViewports = nullptr,
        .scissorCount = 1,
        .pScissors = nullptr
        };

        std::vector<vk::DynamicState> dynamicStates = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };
        vk::PipelineDynamicStateCreateInfo dynamicState{ .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()), .pDynamicStates = dynamicStates.data() };

        vk::PipelineColorBlendStateCreateInfo colorBlending{ .logicOpEnable = vk::False, .logicOp = vk::LogicOp::eCopy, .attachmentCount = 1, .pAttachments = &m_colorBlendAttachment };

        vk::PipelineVertexInputStateCreateInfo vertexInput{};

        vk::GraphicsPipelineCreateInfo pipelineInfo{
                .stageCount = static_cast<uint32_t>(m_shaderStages.size()),
                .pStages = m_shaderStages.data(),
                .pVertexInputState = &vertexInput,
                .pInputAssemblyState = &m_inputAssembly,
                .pViewportState = &viewportState,
                .pRasterizationState = &m_rasterizer,
                .pMultisampleState = &m_multisampling,
                .pDepthStencilState = &m_depthStencil,
                .pColorBlendState = &colorBlending,
                .pDynamicState = &dynamicState,
                .layout = *layout,
                .renderPass = nullptr
        };

        vk::PipelineRenderingCreateInfo renderingInfo{
            .colorAttachmentCount = 1,
            .pColorAttachmentFormats = &colorFormat,
            .depthAttachmentFormat = depthFormat
        };

        vk::StructureChain<vk::GraphicsPipelineCreateInfo, vk::PipelineRenderingCreateInfo> pipelineCreateInfoChain(
            pipelineInfo,
            renderingInfo
        );

        vk::raii::Pipeline pipeline(m_device, nullptr, pipelineCreateInfoChain.get<vk::GraphicsPipelineCreateInfo>());

        return VulkanPipeline(std::move(pipeline), std::move(layout));
    }
}