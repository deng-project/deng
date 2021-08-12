/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: asset_cpy.cpp - asset BLOB to buffer copy helper class implementation for Vulkan
/// author: Karl-Mihkel Ott

#define __VK_ASSET_CPY_CPP
#include <deng/vulkan/asset_cpy.h>


namespace deng {
    namespace vulkan {

        void __AssetCpy::cpyToBuffer(const VkDevice device, const das_Asset &asset, const VkDeviceMemory mem) {
            // Check for the correct method to call based on the asset mode
            switch(asset.asset_mode) {
            case DAS_ASSET_MODE_2D_UNMAPPED:
                __cpyVu2D(device, asset, mem);
                break;

            case DAS_ASSET_MODE_2D_TEXTURE_MAPPED:
                __cpyVm2D(device, asset, mem);
                break;

            case DAS_ASSET_MODE_3D_UNMAPPED:
                __cpyVu3D(device, asset, mem);
                break;
            
            case DAS_ASSET_MODE_3D_TEXTURE_MAPPED:
                __cpyVm3D(device, asset, mem);
                break;
            
            default:
                break;
            }
        }


        void __AssetCpy::bufCpyVertBuffer(const VkDevice device, const VkCommandPool cmd_pool, const das_Asset &asset, 
            const VkBuffer src_buf, const VkBuffer dst_buf, const VkQueue g_queue) {
            // Check for the correct bufCpy method to call based on the asset mode
            switch(asset.asset_mode) {
            case DAS_ASSET_MODE_2D_UNMAPPED:
                __bufCpyVu2DToVertBuffer(device, cmd_pool, asset, src_buf, dst_buf, g_queue);
                break;

            case DAS_ASSET_MODE_2D_TEXTURE_MAPPED:
                __bufCpyVm2DToVertBuffer(device, cmd_pool, asset, src_buf, dst_buf, g_queue);
                break;

            case DAS_ASSET_MODE_3D_UNMAPPED:
                __bufCpyVu3DToVertBuffer(device, cmd_pool, asset, src_buf, dst_buf, g_queue);
                break;

            case DAS_ASSET_MODE_3D_TEXTURE_MAPPED:
                __bufCpyVm3DToVertBuffer(device, cmd_pool, asset, src_buf, dst_buf, g_queue);
                break;

            default:
                break;
            }
        }


        /************************************/
        /* Vertex to buffer copying methods */
        /************************************/
        void __AssetCpy::__cpyVu2D(const VkDevice device, const das_Asset &asset, const VkDeviceMemory mem) {
            // Copy position vertices
            __vk_BufferCreator::cpyToBufferMem(device, asset.vertices.v2d.mul.pn * sizeof(const das_ObjPosData2D),
                asset.vertices.v2d.mul.pos, mem, asset.offsets.pos_offset);

            // Copy indices
            __vk_BufferCreator::cpyToBufferMem(device, asset.indices.n * sizeof(deng_ui32_t),
                asset.indices.pos, mem, asset.offsets.ind_offset);
        }


        void __AssetCpy::__cpyVm2D(const VkDevice device, const das_Asset &asset, const VkDeviceMemory mem) {
            // Copy position vertices
            __vk_BufferCreator::cpyToBufferMem(device, asset.vertices.v2d.mul.pn * sizeof(const das_ObjPosData2D),
                asset.vertices.v2d.mul.pos, mem, asset.offsets.pos_offset);

            // Copy texture vertices
            __vk_BufferCreator::cpyToBufferMem(device, asset.vertices.v2d.mul.tn * sizeof(const das_ObjTextureData),
                asset.vertices.v2d.mul.tex, mem, asset.offsets.tex_offset);

            // Copy position indices
            __vk_BufferCreator::cpyToBufferMem(device, asset.indices.n * sizeof(deng_ui32_t),
                asset.indices.pos, mem, asset.offsets.ind_offset); 

            // Copy texture indices
            __vk_BufferCreator::cpyToBufferMem(device, asset.indices.n * sizeof(deng_ui32_t),
                asset.indices.tex, mem, asset.offsets.ind_offset + asset.indices.n * sizeof(deng_ui32_t));
        }


        void __AssetCpy::__cpyVu3D(const VkDevice device, const das_Asset &asset, const VkDeviceMemory mem) {
            // Copy position vertices
            __vk_BufferCreator::cpyToBufferMem(device, asset.vertices.v3d.mul.pn * sizeof(const das_ObjPosData),
                asset.vertices.v3d.mul.pos, mem, asset.offsets.pos_offset);

            // Copy vertex normals
            __vk_BufferCreator::cpyToBufferMem(device, asset.vertices.v3d.mul.nn * sizeof(const das_ObjNormalData),
                asset.vertices.v3d.mul.norm, mem, asset.offsets.nor_offset);

            // Copy position indices
            __vk_BufferCreator::cpyToBufferMem(device, asset.indices.n * sizeof(deng_ui32_t),
                asset.indices.pos, mem, asset.offsets.ind_offset);

            // Copy vertex normal indices
            __vk_BufferCreator::cpyToBufferMem(device, asset.indices.n * sizeof(deng_ui32_t),
                asset.indices.norm, mem, asset.offsets.ind_offset + asset.indices.n * sizeof(deng_ui32_t));
        }


        void __AssetCpy::__cpyVm3D(const VkDevice device, const das_Asset &asset, const VkDeviceMemory mem) {
            // Copy position vertices
            __vk_BufferCreator::cpyToBufferMem(device, asset.vertices.v3d.mul.pn * sizeof(const das_ObjPosData),
                asset.vertices.v3d.mul.pos, mem, asset.offsets.pos_offset);

            // Copy texture vertices
            __vk_BufferCreator::cpyToBufferMem(device, asset.vertices.v3d.mul.tn * sizeof(const das_ObjTextureData),
                asset.vertices.v3d.mul.tex, mem, asset.offsets.tex_offset); 

            // Copy vertex normals
            __vk_BufferCreator::cpyToBufferMem(device, asset.vertices.v3d.mul.nn * sizeof(const das_ObjNormalData),
                asset.vertices.v3d.mul.norm, mem, asset.offsets.nor_offset);

            // Copy position indices
            __vk_BufferCreator::cpyToBufferMem(device, asset.indices.n * sizeof(deng_ui32_t),
                asset.indices.pos, mem, asset.offsets.ind_offset);

            // Copy texture vertex indices
            __vk_BufferCreator::cpyToBufferMem(device, asset.indices.n * sizeof(deng_ui32_t),
                asset.indices.tex, mem, asset.offsets.ind_offset + asset.indices.n * sizeof(deng_ui32_t));

            // Copy vertex normal indices
            __vk_BufferCreator::cpyToBufferMem(device, asset.indices.n * sizeof(deng_ui32_t),
                asset.indices.norm, mem, asset.offsets.ind_offset + 2 * asset.indices.n * sizeof(deng_ui32_t));
        }


        /***********************************************************/
        /* Single asset data buffer to main buffer copying methods */
        /***********************************************************/
        void __AssetCpy::__bufCpyVu2DToVertBuffer (
            const VkDevice device, 
            const VkCommandPool cmd_pool, 
            const das_Asset &asset,
            const VkBuffer src_buf, 
            const VkBuffer dst_buf, 
            const VkQueue g_queue
        ) {
            // Copy position vertices
            __vk_BufferCreator::cpyBufferToBuffer(device, cmd_pool, g_queue, src_buf, dst_buf,
                asset.vertices.v2d.mul.pn * sizeof(const das_ObjPosData2D), 0, asset.offsets.pos_offset);
        }


        void __AssetCpy::__bufCpyVm2DToVertBuffer (
            const VkDevice device, 
            const VkCommandPool cmd_pool, 
            const das_Asset &asset,
            const VkBuffer src_buf, 
            const VkBuffer dst_buf, 
            const VkQueue g_queue
        ) {
            // Copy position vertices and texture vertices
            __vk_BufferCreator::cpyBufferToBuffer(device, cmd_pool, g_queue, src_buf, dst_buf,
                asset.vertices.v2d.mul.pn * sizeof(das_ObjPosData2D) + asset.vertices.v2d.mul.tn * sizeof(const das_ObjTextureData),
                0, asset.offsets.pos_offset);
        }


        void __AssetCpy::__bufCpyVu3DToVertBuffer (
            const VkDevice device, 
            const VkCommandPool cmd_pool, 
            const das_Asset &asset,
            const VkBuffer src_buf, 
            const VkBuffer dst_buf, 
            const VkQueue g_queue
        ) {
            // Copy position vertices and vertex normals
            __vk_BufferCreator::cpyBufferToBuffer(device, cmd_pool, g_queue, src_buf, dst_buf,
                asset.vertices.v3d.mul.pn * sizeof(das_ObjPosData) + asset.vertices.v3d.mul.nn * sizeof(const das_ObjNormalData),
                0, asset.offsets.pos_offset);
        }

        
        void __AssetCpy::__bufCpyVm3DToVertBuffer (
            const VkDevice device,
            const VkCommandPool cmd_pool,
            const das_Asset &asset,
            const VkBuffer src_buf,
            const VkBuffer dst_buf,
            const VkQueue g_queue
        ) {
            // Copy position vertices, texture vertices and vertex normals
            __vk_BufferCreator::cpyBufferToBuffer(device, cmd_pool, g_queue, src_buf, dst_buf,
                asset.vertices.v3d.mul.pn * sizeof(das_ObjPosData) + asset.vertices.v3d.mul.tn * sizeof(const das_ObjTextureData) +
                asset.vertices.v3d.mul.nn * sizeof(const das_ObjNormalData), 0, asset.offsets.pos_offset);
        }
    }
}
