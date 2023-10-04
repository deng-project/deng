/* Fragment shader */
#if defined(VULKAN)
    #define VULKAN_LOCATION(x) [[vk::location(x)]]
    #define VULKAN_BINDING(x, y) [[vk::binding(x, y)]]
    #define VULKAN_COMBINED_IMAGE_SAMPLER [[vk::combinedImageSampler]]
#else
    #define VULKAN_LOCATION(x)
    #define VULKAN_BINDING(x, y)
    #define VULKAN_COMBINED_IMAGE_SAMPLER
#endif

struct VSInput
{
    VULKAN_LOCATION(0)  float3 Position : POSITION0;
    VULKAN_LOCATION(1)  float3 Normal : NORMAL0;
    VULKAN_LOCATION(2)  nointerpolation int InstanceID : TEXCOORD0;
    VULKAN_LOCATION(7)  float2 NormalTexCoord : TEXCOORD1;
    VULKAN_LOCATION(3)  float2 EmissionTexCoord : TEXCOORD2;
    VULKAN_LOCATION(4)  float2 DiffuseTexCoord : TEXCOORD3;
    VULKAN_LOCATION(5)  float2 SpecularTexCoord : TEXCOORD4;
};

/* Camera data structure */
struct CameraInput
{
    float4x4 Projection;
    float4 CameraRight;
    float4 CameraUp;
    float4 CameraLookAt;
    float4 Position;
};

/* Camera constant buffer */
cbuffer CameraBuffer : register(b0)
{
    CameraInput camera;
};

struct LightingData
{
    float4 Ambient;
    int PointLightCount;
    int DirectionalLightCount;
    int SpotLightCount;
};

cbuffer LightingDataBuffer : register(b1)
{
    LightingData lightingData;
};

/* Light source structures */
struct PointLight
{
    float4 Position;
    float4 Color;
};

struct DirectionalLight
{
    float4 Direction;
    float4 Color;
};

struct SpotLight
{
    float4 Position;
    float4 Direction;
    float4 Color;
    float InnerCutoff;
    float OuterCutoff;
};

/* Misc structures */
struct DrawDescriptorIndices
{
    int TransformIndex;
    int MaterialIndex;
};

struct Material
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float Shininess;
    uint SamplerBits;
};

/* SSBO objects */
StructuredBuffer<DrawDescriptorIndices> drawDescriptorIndices : register(t1);
StructuredBuffer<Material> materials : register(t2);
StructuredBuffer<PointLight> pointLightBuffer : register(t4);
StructuredBuffer<DirectionalLight> dirLightBuffer : register(t5);
StructuredBuffer<SpotLight> spotLightBuffer : register(t6);

/* Texture Samplers */
VULKAN_BINDING(7, 1)
VULKAN_COMBINED_IMAGE_SAMPLER
Texture2D smpNormal : register(t7);
VULKAN_BINDING(7, 1)
VULKAN_COMBINED_IMAGE_SAMPLER
SamplerState smpStateNormal : register(s7);

VULKAN_BINDING(8, 1)
VULKAN_COMBINED_IMAGE_SAMPLER
Texture2D smpAmbient : register(t8);
VULKAN_BINDING(8, 1)
VULKAN_COMBINED_IMAGE_SAMPLER
SamplerState smpStateAmbient : register(s8);

VULKAN_BINDING(9, 1)
VULKAN_COMBINED_IMAGE_SAMPLER
Texture2D smpDiffuse : register(t9);
VULKAN_BINDING(9, 1)
VULKAN_COMBINED_IMAGE_SAMPLER
SamplerState smpStateDiffuse : register(s9);

VULKAN_BINDING(10, 1)
VULKAN_COMBINED_IMAGE_SAMPLER
Texture2D smpSpecular : register(t10);

VULKAN_BINDING(10, 1)
VULKAN_COMBINED_IMAGE_SAMPLER
SamplerState smpStateSpecular : register(t11);

/* Sampler bits definition */
static const uint bUseNormalMapMask    = (1 << 0);
static const uint bUseAmbientMapMask   = (1 << 1);
static const uint bUseDiffuseMapMask   = (1 << 2);
static const uint bUseSpecularMapMask  = (1 << 3);

#define SHINE_MUL 128

static int g_InstanceID;
static int g_MaterialID;
static float3 g_Position;
static float3 g_Normal;

static float3 g_MaterialDiffuse;
static float3 g_MaterialSpecular;

static float3 g_Diffuse = float3(0, 0, 0);
static float3 g_Specular = float3(0, 0, 0);

float3 CalculateDiffuse(in float3 lightDir, in float3 lightDiffuse)
{
    const float diffuseImpact = max(dot(g_Normal, lightDir), 0.0f);
    return lightDiffuse * (diffuseImpact * g_MaterialDiffuse);
}

float3 CalculateSpecular(in float3 lightDir, in float3 lightSpecular)
{
    const float3 viewDir = normalize(camera.Position.xyz - g_Position);
    const float3 reflectDir = reflect(-lightDir, g_Normal);
    const float specularImpact = pow(max(dot(viewDir, reflectDir), 0.0), materials[g_MaterialID].Shininess * SHINE_MUL);
    return lightSpecular * (specularImpact * g_MaterialSpecular);
}

void CalculatePointLights()
{
    // for each point light
    for (uint i = 0; i < lightingData.PointLightCount; i++)
    {
        float3 lightDir = pointLightBuffer[i].Position.xyz - g_Position;
        const float dist = length(lightDir);
        
        // note that we don't use gamma correction here for now
        const float attenuation = 1.0 / dist;
        
        g_Diffuse += attenuation * CalculateDiffuse(lightDir, pointLightBuffer[i].Color.xyz);
        g_Specular += attenuation * CalculateSpecular(lightDir, pointLightBuffer[i].Color.xyz);
    }

}

void CalculateDirectionalLights()
{
    // for each directional light
    for (uint i = 0; i < lightingData.DirectionalLightCount; i++)
    {
        const float3 lightDir = normalize(-dirLightBuffer[i].Direction.xyz);
        g_Diffuse += CalculateDiffuse(lightDir, dirLightBuffer[i].Color.xyz);
        g_Specular += CalculateSpecular(lightDir, dirLightBuffer[i].Color.xyz);
    }

}

void CalculateSpotLights()
{
    // for each spot light
    for (uint i = 0; i < lightingData.SpotLightCount; i++)
    {
        const float3 lightDir = normalize(spotLightBuffer[i].Position.xyz - g_Position);
        const float theta = dot(lightDir, normalize(-spotLightBuffer[i].Direction.xyz));
        const float intensity = clamp((theta - spotLightBuffer[i].OuterCutoff) / 
            max(spotLightBuffer[i].InnerCutoff - spotLightBuffer[i].OuterCutoff, 0.00001), 0.0, 1.0);
    
        g_Diffuse += intensity * CalculateDiffuse(lightDir, spotLightBuffer[i].Color.xyz);
        g_Specular += intensity * CalculateSpecular(lightDir, spotLightBuffer[i].Color.xyz);
    }
}


float3 CalculateAmbient(in float2 diffuseUV)
{
    float3 ambient = float3(0, 0, 0);
    
    if ((materials[g_MaterialID].SamplerBits & bUseAmbientMapMask) != 0)
        ambient = lightingData.Ambient.xyz * materials[g_MaterialID].Ambient.xyz;
    else
        ambient = lightingData.Ambient.xyz * smpDiffuse.Sample(smpStateDiffuse, diffuseUV).xyz;

    return ambient;
}


float4 Main(in VSInput IN) : SV_Target
{
    g_InstanceID = IN.InstanceID;
    g_MaterialID = drawDescriptorIndices[g_InstanceID].MaterialIndex;
    g_Position = IN.Position;
    
    // unmapped or mapped normals
    if ((materials[g_MaterialID].SamplerBits & bUseNormalMapMask) != 0)
        g_Normal = smpNormal.Sample(smpStateNormal, IN.NormalTexCoord).xyz;
    else
        g_Normal = normalize(IN.Normal);
    
    // unmapped or mapped diffuse
    if ((materials[g_MaterialID].SamplerBits & bUseDiffuseMapMask) != 0)
        g_MaterialDiffuse = smpDiffuse.Sample(smpStateDiffuse, IN.DiffuseTexCoord).xyz;
    else
        g_MaterialDiffuse = materials[g_MaterialID].Diffuse.xyz;
    
    // unmapped or mapped specular
    if ((materials[g_MaterialID].SamplerBits & bUseSpecularMapMask) != 0)
        g_MaterialSpecular = smpSpecular.Sample(smpStateSpecular, IN.SpecularTexCoord).xyz;
    else
        g_MaterialSpecular = materials[g_MaterialID].Specular.xyz;
    
    float3 ambient = CalculateAmbient(IN.EmissionTexCoord);
    return float4(ambient + g_Diffuse + g_Specular, 1.0);
}
