/* Fragment shader */
#ifdef VULKAN
    #define VULKAN_LOCATION(x) [[vk::location(x)]]
    #define VULKAN_COMBINED_IMAGE_SAMPLER [[vk::combinedImageSampler]]
#else
    #define VULKAN_LOCATION(x)
    #define VULKAN_COMBINED_IMAGE_SAMPLER
#endif

struct VSInput
{
    VULKAN_LOCATION(0) float2 TexCoord : TEXCOORD0;
    VULKAN_LOCATION(1) float4 Color : COLOR0;
};

VULKAN_COMBINED_IMAGE_SAMPLER Texture2D smpTexture : register(t1);
VULKAN_COMBINED_IMAGE_SAMPLER SamplerState smpTextureSampler : register(s1);

float4 main(in VSInput IN) : SV_Target
{
    return IN.Color * smpTexture.Sample(smpTextureSampler, IN.TexCoord);
}