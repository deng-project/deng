#include "headers/renderer.h"

namespace deng {
    VkVertexInputBindingDescription VertexInputDesc::getBindingDesc(GameObject &obj) {
        VkVertexInputBindingDescription local_input_binding_desc{};
        local_input_binding_desc.binding = 0;
        local_input_binding_desc.stride = sizeof(ObjVertexData);
        local_input_binding_desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return local_input_binding_desc;
    } 

    std::array<VkVertexInputAttributeDescription, 3> VertexInputDesc::getAttributeDesc() {
        std::array<VkVertexInputAttributeDescription, 3> local_input_attr_desc{};

        local_input_attr_desc[0].binding = 0;
        local_input_attr_desc[0].location = 0;
        local_input_attr_desc[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        local_input_attr_desc[0].offset = offsetof(ObjVertexData, posVec);

        local_input_attr_desc[1].binding = 0;
        local_input_attr_desc[1].location = 1;
        local_input_attr_desc[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        local_input_attr_desc[1].offset = offsetof(ObjVertexData, colorVec);

        local_input_attr_desc[2].binding = 0;
        local_input_attr_desc[2].location = 2;
        local_input_attr_desc[2].format = VK_FORMAT_R32G32_SFLOAT;
        local_input_attr_desc[2].offset = offsetof(ObjVertexData, texVec);

        return local_input_attr_desc;
    }

    ObjRawTextureData::ObjRawTextureData() {
        this->width = new uint32_t;
        this->height = new uint32_t;
        this->texSize = new VkDeviceSize;
    }

    void ObjRawTextureData::cpyDims(ObjTextureData &texData) {
        texData.width = *this->width;
        texData.height = *this->height; 
    }

    void ObjRawTextureData::clear() {
        this->texturePixelsData.clear();
        delete this->height;
        delete this->width;
        delete this->texSize;
    }
}