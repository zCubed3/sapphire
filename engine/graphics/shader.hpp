/*
MIT License

Copyright (c) 2023 zCubed (Liam R.)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef SAPPHIRE_SHADER_HPP
#define SAPPHIRE_SHADER_HPP

#include <graphics/provider_releasable.hpp>

#include <vulkan/vulkan.h>

#include <vector>
#include <string>
#include <memory>

namespace Sapphire::Graphics {
    enum class CullMode : int {
        Off,
        Back,
        Front
    };

    enum class WindingOrder : int {
        Clockwise,
        CounterClockwise
    };

    enum class FillMode : int {
        Point,
        Line,
        Face
    };

    enum ColorMaskFlags : int {
        ColorMaskNone = 0,
        ColorMaskAll = ~0,

        ColorMaskR = 1,
        ColorMaskG = 2,
        ColorMaskB = 4,
        ColorMaskA = 8
    };

    enum class ColorBlendMode : int {
        None,
        Zero,
        One
    };

    enum class ColorBlendOp : int {
        None,
        Add
    };

    enum class AlphaBlendMode : int {
        None,
        Zero,
        One
    };

    enum class AlphaBlendOp : int {
        None,
        Add
    };

    enum class DepthCompareOp : int {
        Less,
        LessOrEqual,
        Greater,
        GreaterOrEqual,
        Equal,
        Always
    };

    class ShaderModule {
        //friend class Shader;

    public:
        enum ModuleType {
            Vertex,
            Fragment
        };

    protected:
        VkPipelineShaderStageCreateInfo vk_stage_info {};
        VkShaderModule vk_module = nullptr;

        ModuleType module_type = ModuleType::Vertex;
        std::vector<char> data;
        std::string entry_point;

        // Passes the SPIR-V binary into our vulkan instance and readies it for usage with a Shader
        void compile(VulkanProvider *p_provider);

    public:
        ShaderModule() = delete;

        // Creates a shader module but doesn't compile it immediately
        ShaderModule(ModuleType type, std::vector<char> data, std::string entry_point = "main");

        // Creates a shader module and compiles it immediately
        ShaderModule(VulkanProvider *p_provider, ModuleType type, std::vector<char> data, std::string entry_point = "main");

        VkPipelineShaderStageCreateInfo get_vk_stage_info();
    };

    struct ShaderProperties {
        WindingOrder winding_order = WindingOrder::Clockwise;
        CullMode cull_mode = CullMode::Back;
        FillMode fill_mode = FillMode::Face;
        bool allow_discard = false;
        bool clamp_depth = false;
        int color_mask_flags = ColorMaskFlags::ColorMaskAll;

        bool depth_test = true;
        bool depth_write = true;
        DepthCompareOp depth_compare_op = DepthCompareOp::Less;

        ColorBlendMode color_src_blend_mode = ColorBlendMode::None;
        ColorBlendMode color_dst_blend_mode = ColorBlendMode::None;
        ColorBlendOp color_blend_op = ColorBlendOp::None;

        AlphaBlendMode alpha_src_blend_mode = AlphaBlendMode::None;
        AlphaBlendMode alpha_dst_blend_mode = AlphaBlendMode::None;
        AlphaBlendOp alpha_blend_op = AlphaBlendOp::None;
    };

    // TODO: Shader variants?
    // TODO: Geometry shaders?
    // TODO: Compute shaders?
    class Shader : public IProviderReleasable {
    protected:
        std::function<void (VulkanProvider *)> get_release_func() override;

        VkPipeline vk_pipeline = nullptr;
        VkPipelineLayout vk_pipeline_layout = nullptr;

        void compile(VulkanProvider *p_provider, ShaderProperties properties, const std::vector<std::shared_ptr<ShaderModule>>& shader_modules);

    public:
        Shader() = delete;
        Shader(VulkanProvider *p_provider, ShaderProperties properties, const std::shared_ptr<ShaderModule>& sm_vertex, const std::shared_ptr<ShaderModule>& sm_fragment);

        void bind(VkCommandBuffer vk_cmd_buffer);
    };
}

#endif//SAPPHIRE_SHADER_HPP
