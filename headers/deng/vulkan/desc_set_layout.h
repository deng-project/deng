/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: desc_set_layout.h - Vulkan descriptor set layout creator class header
/// author: Karl-Mihkel Ott


#ifndef __VK_DESC_SET_LAYOUT_H
#define __VK_DESC_SET_LAYOUT_H


#ifdef __VK_DESC_SET_LAYOUT_CPP
    #include <vector>
    #include <string>
    #include <vulkan/vulkan.h>
    
    #include <common/base_types.h>
    #include <common/err_def.h>
    #include <data/assets.h>

    #include <math/deng_math.h>
    #include <deng/window.h>
    #include <deng/vulkan/sd.h>
    #include <deng/vulkan/qm.h>
    #include <deng/vulkan/resources.h>
    #include <deng/vulkan/rend_infos.h>
#endif


namespace deng {
    namespace vulkan {

        class __vk_DescriptorSetLayoutCreator {
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
    }
}

#endif
