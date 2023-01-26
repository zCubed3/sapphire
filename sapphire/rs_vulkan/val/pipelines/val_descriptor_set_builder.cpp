#include "val_descriptor_set_builder.h"

#include <rs_vulkan/val/val_instance.h>
#include <rs_vulkan/val/pipelines/val_descriptor_set.h>

void ValDescriptorSetBuilderSetInfo::push_binding(VkDescriptorSetLayoutBinding vk_binding) {
    bindings.push_back(vk_binding);

    layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layout_info.bindingCount = static_cast<uint32_t>(bindings.size());
    layout_info.pBindings = bindings.data();
}

uint32_t ValDescriptorSetBuilderSetInfo::get_open_location(uint32_t vk_stage_flags) {
    uint32_t location = 0;
    bool collision = true;
    while (collision) {
        collision = false;

        for (VkDescriptorSetLayoutBinding binding: bindings) {
            if (binding.binding == location) {
                if (binding.stageFlags & vk_stage_flags) {
                    collision = true;
                    break;
                }
            }
        }

        if (collision) {
            location++;
        }
    }

    return location;
}

VkDescriptorSetLayout ValDescriptorSetBuilderSetInfo::build(ValInstance *p_val_instance) {
    VkDescriptorSetLayout vk_descriptor_layout = nullptr;

    if (vkCreateDescriptorSetLayout(p_val_instance->vk_device, &layout_info, nullptr, &vk_descriptor_layout) != VK_SUCCESS) {
        return nullptr;
    }

    return vk_descriptor_layout;
}

void ValDescriptorSetBuilder::push_set() {
    sets.emplace_back();
}

void ValDescriptorSetBuilder::push_binding(VkDescriptorType vk_type, uint32_t count, uint32_t vk_stage_flags) {
    if (sets.empty()) {
        push_set();
    }

    // We need to find a binding location within this set that doesn't collide with any other sets
    // TODO: Will this break shaders that expect a certain layout?
    uint32_t location = sets.back().get_open_location(vk_stage_flags);

    VkDescriptorSetLayoutBinding layout_binding{};
    layout_binding.binding = location;
    layout_binding.descriptorType = vk_type;
    layout_binding.descriptorCount = count;
    layout_binding.stageFlags = vk_stage_flags;
    layout_binding.pImmutableSamplers = nullptr; // TODO: Immutable samplers?

    sets.back().push_binding(layout_binding);
}

ValDescriptorSet* ValDescriptorSetBuilder::build(ValInstance *p_val_instance) {
    std::vector<VkDescriptorSetLayout> vk_descriptor_set_layouts;

    for (ValDescriptorSetBuilderSetInfo& set: sets) {
        VkDescriptorSetLayout layout = set.build(p_val_instance);

        // TODO: Error reporting
        if (layout == nullptr) {
            for (VkDescriptorSetLayout& vk_descriptor_set_layout: vk_descriptor_set_layouts) {
                vkDestroyDescriptorSetLayout(p_val_instance->vk_device, vk_descriptor_set_layout, nullptr);
            }

            return nullptr;
        }


        vk_descriptor_set_layouts.push_back(layout);
    }

    // TODO: Abstract descriptor pools
    VkDescriptorSetAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = p_val_instance->vk_descriptor_pool;
    alloc_info.descriptorSetCount = static_cast<uint32_t>(vk_descriptor_set_layouts.size());
    alloc_info.pSetLayouts = vk_descriptor_set_layouts.data();

    std::vector<VkDescriptorSet> vk_descriptor_sets;
    vk_descriptor_sets.resize(alloc_info.descriptorSetCount);

    if (vkAllocateDescriptorSets(p_val_instance->vk_device, &alloc_info, vk_descriptor_sets.data()) != VK_SUCCESS) {
        for (VkDescriptorSetLayout& vk_descriptor_set_layout: vk_descriptor_set_layouts) {
            vkDestroyDescriptorSetLayout(p_val_instance->vk_device, vk_descriptor_set_layout, nullptr);
        }

        return nullptr;
    }

    ValDescriptorSet* val_descriptor_set = new ValDescriptorSet();
    val_descriptor_set->vk_descriptor_sets = vk_descriptor_sets;
    val_descriptor_set->vk_descriptor_set_layouts = vk_descriptor_set_layouts;

    return val_descriptor_set;
}