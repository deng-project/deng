/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: desc_set_layout.cpp - Vulkan descriptor set layout creator class implementation
/// author: Karl-Mihkel Ott


#define __VK_DESC_SET_LAYOUT_CPP
#include <deng/vulkan/desc_set_layout.h>


namespace deng {
    namespace vulkan {

        __vk_DescriptorSetLayoutCreator::__vk_DescriptorSetLayoutCreator(VkDevice device, void *udata) : m_udata(udata) {
            // Create all Vulkan descriptor layouts
            __mkVu2DLayout(device);
            __mkVm2DLayout(device);
            __mkVu3DLayout(device);
            __mkVm3DLayout(device);
            __mkUILayout(device);
        }


        /// Find the binding data according to the asset mode
        std::vector<VkDescriptorSetLayoutBinding> __vk_DescriptorSetLayoutCreator::__findBindings(deng_PipelineType pl_type) {
            std::vector<VkDescriptorSetLayoutBinding> bindings;
            bindings.reserve(4);
            bindings.resize(1);

            // Check if the pipeline type is not UI one
            if(pl_type != DENG_PIPELINE_TYPE_UI) {
                // Transformation uniform data
                bindings[0].binding = 0;
                bindings[0].descriptorCount = 1;
                bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                bindings[0].pImmutableSamplers = NULL;
                bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

                // Check if any other binding are needed according to the asset mode
                switch(pl_type) {
                case DENG_PIPELINE_TYPE_UNMAPPED_2D:
                    bindings.resize(2);
                    // Asset uniform data
                    bindings[1].binding = 1;
                    bindings[1].descriptorCount = 1;
                    bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                    bindings[1].pImmutableSamplers = NULL;
                    bindings[1].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
                    break;

                case DENG_PIPELINE_TYPE_TEXTURE_MAPPED_2D:
                    bindings.resize(3);

                    // Asset uniform data
                    bindings[1].binding = 1;
                    bindings[1].descriptorCount = 1;
                    bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                    bindings[1].pImmutableSamplers = NULL;
                    bindings[1].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

                    // Texture image sampler
                    bindings[2].binding = 2;
                    bindings[2].descriptorCount = 1;
                    bindings[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                    bindings[2].pImmutableSamplers = NULL;
                    bindings[2].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
                    break;

                case DENG_PIPELINE_TYPE_UNMAPPED_3D:
                    bindings.resize(3);

                    // Asset uniform data
                    bindings[1].binding = 1;
                    bindings[1].descriptorCount = 1;
                    bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                    bindings[1].pImmutableSamplers = NULL;
                    bindings[1].stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
                    
                    // Lighting data
                    bindings[2].binding = 2;
                    bindings[2].descriptorCount = 1;
                    bindings[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                    bindings[2].pImmutableSamplers = NULL;
                    bindings[2].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
                    break;

                case DENG_PIPELINE_TYPE_TEXTURE_MAPPED_3D:
                    bindings.resize(4);

                    // Asset uniform data
                    bindings[1].binding = 1;
                    bindings[1].descriptorCount = 1;
                    bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                    bindings[1].pImmutableSamplers = NULL;
                    bindings[1].stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
                    
                    // Lighting data
                    bindings[2].binding = 2;
                    bindings[2].descriptorCount = 1;
                    bindings[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                    bindings[2].pImmutableSamplers = NULL;
                    bindings[2].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

                    // Texture image sampler
                    bindings[3].binding = 3;
                    bindings[3].descriptorCount = 1;
                    bindings[3].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                    bindings[3].pImmutableSamplers = NULL;
                    bindings[3].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
                    break;

                default:
                    break;
                }
            }

            // Setup UI pipeline bindings
            else {
                bindings[0].binding = 0;
                bindings[0].descriptorCount = 1;
                bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                bindings[0].pImmutableSamplers = NULL;
                bindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
            }

            return bindings;
        }


        /// Create new descriptor set layout
        void __vk_DescriptorSetLayoutCreator::__mkGenericDescSetLayout (
            VkDevice device, 
            std::vector<VkDescriptorSetLayoutBinding> &bindings,
            VkDescriptorSetLayout *p_layout
        ) {
            VkDescriptorSetLayoutCreateInfo createinfo{};
            createinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            createinfo.bindingCount = static_cast<deng_ui32_t>(bindings.size());
            createinfo.pBindings = bindings.data();

            SET_UDATA("vkCreateDescriptorSetLayout");
            if(vkCreateDescriptorSetLayout(device, &createinfo, NULL, p_layout) != VK_SUCCESS)
                VK_DESC_ERR("failed to create descriptor set layout");
        }


        void __vk_DescriptorSetLayoutCreator::__mkVu2DLayout(VkDevice device) {
            // Find the layout bindings
            std::vector<VkDescriptorSetLayoutBinding> bindings = __findBindings(
                DENG_PIPELINE_TYPE_UNMAPPED_2D);
            LOG("2D unmapped binding count: " + std::to_string(bindings.size()));

            // Create the layout
            __mkGenericDescSetLayout(device, bindings, &m_vu2d_layout);
        }


        void __vk_DescriptorSetLayoutCreator::__mkVm2DLayout(VkDevice device) {
            // Find the layout bindings
            std::vector<VkDescriptorSetLayoutBinding> bindings = __findBindings(
                DENG_PIPELINE_TYPE_TEXTURE_MAPPED_2D);
            LOG("2D texture mapped binding count: " + std::to_string(bindings.size()));

            // Create the layout
            __mkGenericDescSetLayout(device, bindings, &m_vm2d_layout);
        }


        void __vk_DescriptorSetLayoutCreator::__mkVu3DLayout(VkDevice device) {
            // Find the layout bindings
            std::vector<VkDescriptorSetLayoutBinding> bindings = __findBindings(
                DENG_PIPELINE_TYPE_UNMAPPED_3D);
            LOG("3D unmapped binding count: " + std::to_string(bindings.size()));

            // Create the layout
            __mkGenericDescSetLayout(device, bindings, &m_vu3d_layout);
        }


        void __vk_DescriptorSetLayoutCreator::__mkVm3DLayout(VkDevice device) {
            // Find the layout bindings
            std::vector<VkDescriptorSetLayoutBinding> bindings = __findBindings(
                DENG_PIPELINE_TYPE_TEXTURE_MAPPED_3D);
            LOG("3D texture mapped binding count: " + std::to_string(bindings.size()));

            // Create the layout
            __mkGenericDescSetLayout(device, bindings, &m_vm3d_layout);
        }


        void __vk_DescriptorSetLayoutCreator::__mkUILayout(VkDevice device) {
            // Find the layout bindings
            std::vector<VkDescriptorSetLayoutBinding> bindings = __findBindings(
                DENG_PIPELINE_TYPE_UI);
            LOG("UI binding count: " + std::to_string(bindings.size()));

            // Create the layout
            __mkGenericDescSetLayout(device, bindings, &m_ui_layout);
        }


        VkDescriptorSetLayout &__vk_DescriptorSetLayoutCreator::getLayout(deng_PipelineType pl_type) {
            // Check the asset mode and return correct layout
            switch(pl_type) {
                case DENG_PIPELINE_TYPE_UNMAPPED_2D:        return m_vu2d_layout;
                case DENG_PIPELINE_TYPE_TEXTURE_MAPPED_2D:  return m_vm2d_layout;
                case DENG_PIPELINE_TYPE_UNMAPPED_3D:        return m_vu3d_layout;
                case DENG_PIPELINE_TYPE_TEXTURE_MAPPED_3D:  return m_vm3d_layout;
                case DENG_PIPELINE_TYPE_UI:                 return m_ui_layout;

                default:
                    RUN_ERR("__vk_DescriptorSetLayoutCreator::getLayout()", 
                        "Invalid pipeline type value");
                    break;
            }
        }
    }
}
