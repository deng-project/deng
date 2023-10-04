/* Fragment shader */
#if defined(VULKAN)
    #define VULKAN_LOCATION(x) [[vk::location(x)]]
    #define VULKAN_BINDING(x, y) [[vk::binding(x, y)]]
    #define VULKAN_COMBINED_IMAGE_SAMPLER [[vk::combinedImageSampler]]
#else
    #define VULKAN_LOCATION(x)
    #define VULKAN_BINDING(x)
    #define VULKAN_COMBINED_IMAGE_SAMPLER
#endif

struct VSInput
{
    VULKAN_LOCATION(0)  float3 Position : POSITION0;
    VULKAN_LOCATION(1)  float3 Normal : NORMAL0;
    
};