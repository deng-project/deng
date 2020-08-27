#ifndef OBJECTSHANDLER_H
#define OBJECTSHANDLER_H
#include "camera.h"

namespace deng {

    struct ObjVertexData {
        vec3<float> posVec;
        vec2<float> texVec;

        static VkVertexInputBindingDescription getBindingDesc();
        static std::array<VkVertexInputAttributeDescription, 2> getAttributeDesc();
    };

    struct ObjVertexIndicesData {
        std::vector<uint32_t> posIndices;
        std::vector<uint32_t> texIndices;
    };

    struct Buffers {
        VkBuffer vertex_buffer;
        VkDeviceMemory vertex_bufferMem;

        VkBuffer staging_buffer;
        VkDeviceMemory staging_bufferMem;

        VkBuffer index_buffer;
        VkDeviceMemory index_bufferMem;
    };

    struct DepthImageData {
        VkImage depthImage;
        VkDeviceMemory depthImageMem;
        VkImageView depthImageView;
    };

    struct ObjTextureData {
        uint32_t width;
        uint32_t height;

        VkImage textureImage;
        VkDeviceMemory textureImageMem;
        VkImageView textureImageView;
        VkSampler textureSampler;
    };

    struct ObjRawTextureData {
        std::vector<uint8_t> texturePixelsData;

        uint32_t *width;
        uint32_t *height;
        VkDeviceSize *texSize;
        ObjRawTextureData();

    public:
        void cpyDims(ObjTextureData &texData);
        void clear();
    };

    struct GameObject {
        vec3<float> origin;
        std::vector<ObjVertexData> vertexData;
        ObjVertexIndicesData vertexIndicesData;
        ObjRawTextureData rawTextureData;
        ObjTextureData textureData;
        ModelMatrix modelMatrix;        
    };
}

#endif