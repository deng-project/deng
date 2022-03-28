// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanDescriptorSetLayoutCreator.h - Vulkan descriptor set layout creator class header
// author: Karl-Mihkel Ott


#ifndef VULKAN_DESCRIPTOR_SET_LAYOUT_CREATOR_H
#define VULKAN_DESCRIPTOR_SET_LAYOUT_CREATOR_H


#ifdef VULKAN_DESCRIPTOR_SET_LAYOUT_CREATOR_CPP
    #include <vector>
    #include <string>
    #include <cstring>
#ifdef _DEBUG
    #include <iostream>
#endif
    #include <vulkan/vulkan.h>

    #include <libdas/include/Points.h>
    #include <libdas/include/Vector.h>
    #include <libdas/include/Matrix.h>

    #include <ErrorDefinitions.h>
    #include <ShaderDefinitions.h>
    #include <Api.h>
    #include <Window.h>
    #include <Renderer.h>
#endif


namespace DENG {

    namespace Vulkan {

        class DescriptorSetLayoutCreator {
            private:
                VkDevice m_device;
                ShaderModule const *m_shader_module;
                MeshReference const *m_sample_mesh;
                VkDescriptorSetLayout m_descriptor_set_layout = VK_NULL_HANDLE;

            private:
                void _CreateDescriptorSetLayout();

            public:
                DescriptorSetLayoutCreator(VkDevice _dev, const ShaderModule &_module, const MeshReference &_mesh);
                DescriptorSetLayoutCreator(const DescriptorSetLayoutCreator &_dslc) = default;
                DescriptorSetLayoutCreator(DescriptorSetLayoutCreator &&_dslc);
                ~DescriptorSetLayoutCreator();

                DescriptorSetLayoutCreator &operator=(const DescriptorSetLayoutCreator &_dslc);
                DescriptorSetLayoutCreator &operator=(DescriptorSetLayoutCreator &&_dslc);

                inline VkDescriptorSetLayout GetDescriptorSetLayout() { return m_descriptor_set_layout; }
        };
    }
}

#endif
