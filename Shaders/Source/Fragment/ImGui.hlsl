/* Fragment shader */
#ifdef VULKAN
    #define VULKAN_LOCATION(x) [[vk::location(x)]]
#else
    #define VULKAN_LOCATION(x)
#endif
struct VSInput
{
    VULKAN_LOCATION(0) float2 TexCoord : TEXCOORD0;
    VULKAN_LOCATION(1) float4 Color : COLOR0;
};

Texture2D smpTexture : register(t0);
SamplerState smpTextureSampler : register(s0);

float4 Main(in VSInput IN) : SV_Target
{
    return IN.Color * smpTexture.Sample(smpTextureSampler, IN.TexCoord);
}