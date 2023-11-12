#pragma once

#include <vk_types.h>

class PipelineBuilder {
public:
    std::vector<VkPipelineShaderStageCreateInfo> _shaderStages;
    std::vector<VkFormat> _colorAttachmentformats;

    VkPipelineVertexInputStateCreateInfo _vertexInputInfo;
    VkPipelineInputAssemblyStateCreateInfo _inputAssembly;   
    VkPipelineRasterizationStateCreateInfo _rasterizer;
    VkPipelineColorBlendAttachmentState _colorBlendAttachment;
    VkPipelineMultisampleStateCreateInfo _multisampling;
    VkPipelineLayout _pipelineLayout;
    VkPipelineDepthStencilStateCreateInfo _depthStencil;
    VkPipelineRenderingCreateInfo _renderInfo;

    PipelineBuilder();

    void clear();

    VkPipeline build_pipeline(VkDevice device);

    void set_shaders(VkShaderModule vertexShader, VkShaderModule fragmentShader);
    void set_empty_vertex_input();

    void set_input_topology(VkPrimitiveTopology topology);

    void set_polygon_mode(VkPolygonMode mode);

    void set_cull_mode(VkCullModeFlags cullMode, VkFrontFace frontFace);

    void set_multisampling_none();

    void disable_blending();
    void enable_blending_additive();

    void set_color_attachment_formats(std::span<VkFormat> formats);

    void set_depth_format(VkFormat format);

    void disable_depthtest();

    void enable_depthtest(bool depthWriteEnable,VkCompareOp op);
};

namespace vkutil {
bool load_shader_module(const char* filePath, VkDevice device, VkShaderModule* outShaderModule);
}
