// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanDescriptorSetLayoutCreator.h - Vulkan descriptor set layout creator class header
// author: Karl-Mihkel Ott


#ifndef VULKAN_DESCRIPTOR_SET_LAYOUT_CREATOR_H
#define VULKAN_DESCRIPTOR_SET_LAYOUT_CREATOR_H


#ifdef VULKAN_DESCRIPTOR_SET_LAYOUT_CREATOR_CPP
    #include <vector>
    #include <string>
    #include <vulkan/vulkan.h>

    #include <ErrorDefinitions.h>
    #include <ShaderDefinitions.h>
#endif


namespace DENG {

    namespace Vulkan {

        class DescriptorSetLayoutCreator {
            private:
                VkDevice m_device;
                std::vector<VkDescriptorSetLayout> m_descriptor_set_layouts;

            private:
                void _CreateDescriptorSetLayout(const std::vector<UniformDataLayout> &_layouts);

            public:
                DescriptorSetLayoutCreator(VkDevice _dev, const std::vector<ShaderModule*> &_modules);
                ~DescriptorSetLayoutCreator();

                inline VkDescriptorSetLayout GetLayoutSetById(uint32_t _id) { return m_descriptor_set_layouts[_id]; }
                inline std::vector<VkDescriptorSetLayout> &GetLayouts() { return m_descriptor_set_layouts; }
        };

#if 0
        class DescriptorSetLayoutCreator {
        private:
            VkDescriptorSetLayout m_vu2d_layout;
            VkDescriptorSetLayout m_vm2d_layout;
            VkDescriptorSetLayout m_vu3d_layout;
            VkDescriptorSetLayout m_vm3d_layout;
            VkDescriptorSetLayout m_ui_layout;
            void *m_udata;

        private:
            /// Find the binding data according to the asset mode
            std::vector<VkDescriptorSetLayoutBinding> __findBindings(deng_PipelineType pl_type);

            /// Create new descriptor set layout
            void __mkGenericDescSetLayout(VkDevice device, std::vector<VkDescriptorSetLayoutBinding> &bindings, 
                VkDescriptorSetLayout *p_layout);
            
            /// Create descriptor set layouts for different asset types
            void __mkVu2DLayout(VkDevice device);
            void __mkVm2DLayout(VkDevice device);
            void __mkVu3DLayout(VkDevice device);
            void __mkVm3DLayout(VkDevice device);
            void __mkUILayout(VkDevice device);

        public:
            __vk_DescriptorSetLayoutCreator(VkDevice device, void *udata);

        /// Layout getter
        public:
            VkDescriptorSetLayout &getLayout(deng_PipelineType pl_type);
        };
#endif
    }
}

#endif
