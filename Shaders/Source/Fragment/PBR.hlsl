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
    VULKAN_LOCATION(3)  float2 AlbedoTexCoord : TEXCOORD1;
    VULKAN_LOCATION(4)  float2 EmissionTexCoord : TEXCOORD2;
    VULKAN_LOCATION(5)  float2 NormalTexCoord : TEXCOORD3;
    VULKAN_LOCATION(6)  float2 MetallicTexCoord : TEXCOORD4;
    VULKAN_LOCATION(7)  float2 RoughnessTexCoord : TEXCOORD5;
    VULKAN_LOCATION(8)  float2 AmbientOcclusionTexCoord : TEXCOORD6;
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
    float4 Albedo;
    float4 Emission;
    float Roughness;
    float Metallic;
    float AmbientOcclusion;
    uint SamplerBits;
};


/* SSBO objects */
StructuredBuffer<PointLight> pointLightBuffer : register(t1);
StructuredBuffer<DirectionalLight> dirLightBuffer : register(t2);
StructuredBuffer<SpotLight> spotLightBuffer : register(t3);
StructuredBuffer<DrawDescriptorIndices> drawDescriptorIndices : register(t4);
StructuredBuffer<Material> materials : register(t5);

/* Sampler bitmask definitions */
static const uint g_AlbedoMapMask              = (1 << 0);
static const uint g_EmissionMapMask            = (1 << 1);
static const uint g_NormalMapMask              = (1 << 2);
static const uint g_MetallicMapMask            = (1 << 3);
static const uint g_RoughnessMapMask           = (1 << 4);
static const uint g_AmbientOcclusionMapMask    = (1 << 5);


/* Texture samplers */
VULKAN_BINDING(6, 1) 
VULKAN_COMBINED_IMAGE_SAMPLER
Texture2D smpAlbedo : register(t6);
VULKAN_BINDING(6, 1)  
VULKAN_COMBINED_IMAGE_SAMPLER
SamplerState smpStateAlbedo : register(s6);

VULKAN_BINDING(7, 1)
VULKAN_COMBINED_IMAGE_SAMPLER
Texture2D smpEmission : register(t7);
VULKAN_BINDING(7, 1) 
VULKAN_COMBINED_IMAGE_SAMPLER
SamplerState smpStateEmission : register(s7);

VULKAN_BINDING(8, 1)
VULKAN_COMBINED_IMAGE_SAMPLER
Texture2D smpNormal : register(t8);
VULKAN_BINDING(8, 1) 
VULKAN_COMBINED_IMAGE_SAMPLER
SamplerState smpStateNormal : register(s8);

VULKAN_BINDING(9, 1)
VULKAN_COMBINED_IMAGE_SAMPLER
Texture2D smpMetallic : register(t9);
VULKAN_BINDING(9, 1) 
VULKAN_COMBINED_IMAGE_SAMPLER
SamplerState smpStateMetallic : register(s9);

VULKAN_BINDING(10, 1)
VULKAN_COMBINED_IMAGE_SAMPLER
Texture2D smpRoughness : register(t10);
VULKAN_BINDING(10, 1) 
VULKAN_COMBINED_IMAGE_SAMPLER
SamplerState smpStateRoughness : register(s10);

VULKAN_BINDING(11, 1)
VULKAN_COMBINED_IMAGE_SAMPLER
Texture2D smpAmbientOcclusion : register(t11);
VULKAN_BINDING(11, 1) 
VULKAN_COMBINED_IMAGE_SAMPLER
SamplerState smpStateAmbientOcclusion : register(s11);


/* Some utility macros */
#define SQ(x) ((x)*(x))
#define PI 3.141592653589793f

/* Global variables */
static int g_InstanceIndex = 0;
static int g_MaterialIndex = 0;
static float3 g_View;
static float3 g_InputPosition;

static float3 g_Albedo;
static float3 g_Emission;
static float3 g_Normal;
static float g_Metallic;
static float g_Roughness;
static float g_AmbientOcclusion;

static float3 g_F0 = float3(0.0, 0.0, 0.0);
static float g_Alpha = 0.f;
static float g_K = 0.0;

// lambertian function
float3 Lambert(in float3 color)
{
    return color / PI;
}

// GGX/Throwbridge-Reitz normal distribution function
float D(in float3 H)
{
    const float sqAlpha = SQ(g_Alpha);
    const float num = sqAlpha;
    
    const float NHDot = max(dot(g_Normal, H), 0.0);
    const float denom = max(PI * SQ(SQ(NHDot) * (sqAlpha - 1.0) + 1.0), 0.00001);
    
    return num / denom;
}

float G1(in float3 X)
{
    const float NXDot = max(dot(g_Normal, X), 0.0);
    const float num = NXDot;
    const float denom = max(NXDot * (1 - g_K) * g_K, 0.00001);
    
    return num / denom;
}

// Schlick-GGX, combination of Smith and Schlick-Beckmann geometry shadowing functions
float G(in float3 lightDir)
{
    return G1(lightDir) * G1(g_View);
}

// fresnel function
float3 F(in float3 H)
{
    return g_F0 + (float3(1.0, 1.0, 1.0) - g_F0) * pow(1 - max(dot(H, g_View), 0.0), 5.0);
}

float3 Cook_Torrence(in float3 lightDir, in float3 H)
{
    float3 num = D(H) * G(lightDir) * F(H);
    float denom = max(4.0 * max(dot(g_View, g_Normal), 0.0) * max(dot(lightDir, g_Normal), 0.0), 0.00001);
    return num / denom;
}

/* Bidirectional reflectance distribution function */
float3 BRDF(in float3 lightDir, in float3 color)
{
    const float3 H = normalize(lightDir + g_View);
    const float3 Kd = (float3(1.0, 1.0, 1.0) - F(H) * (1.0 - g_Metallic));
    return Kd * Lambert(color) + Cook_Torrence(lightDir, H);
}

/* Light specific PBR functions */
float3 CalculatePointLights()
{
    float3 output = float3(0.0, 0.0, 0.0);
    
    // for each point light
    for (uint i = 0; i < lightingData.PointLightCount; i++)
    {
        const float3 lightDir = normalize(pointLightBuffer[i].Position.xyz - g_InputPosition);
        const float dist = length(pointLightBuffer[i].Position.xyz - g_InputPosition);
        const float attenuation = 1 / SQ(dist);
        
        const float3 Li = attenuation * pointLightBuffer[i].Color.xyz;
        const float d = max(dot(lightDir, g_Normal), 0.0);
        const float3 Fr = BRDF(lightDir, g_Albedo);
        output += (Li * Fr * d);
    }

    return output;
}


float3 CalculateSpotLights()
{
    float3 output = float3(0.0, 0.0, 0.0);
    
    // for each spot light
    for (uint i = 0; i < lightingData.SpotLightCount; i++)
    {
        const float3 lightDir = normalize(spotLightBuffer[i].Position.xyz - g_InputPosition);
        const float theta = max(dot(lightDir, normalize(-spotLightBuffer[i].Direction.xyz)), 0.0);
        
        const float num = theta - spotLightBuffer[i].OuterCutoff;
        const float denom = max(spotLightBuffer[i].InnerCutoff - spotLightBuffer[i].OuterCutoff, 0.00001);
        const float intensity = clamp(num / denom, 0.0, 1.0);
        
        const float3 Li = intensity * spotLightBuffer[i].Color.xyz;
        const float d = max(dot(lightDir, g_Normal), 0.0);
        const float3 Fr = BRDF(lightDir, g_Albedo);
        
        output += (Li * Fr * d);
    }

    return output;
}


float3 CalculateDirectionalLights()
{
    float3 output = float3(0.0, 0.0, 0.0);
    
    for (uint i = 0; i < lightingData.DirectionalLightCount; i++)
    {
        const float3 lightDir = normalize(-dirLightBuffer[i].Direction.xyz);
        const float3 Li = dirLightBuffer[i].Color.xyz;
        const float d = max(dot(lightDir, g_Normal), 0.0);
        const float3 Fr = BRDF(lightDir, g_Albedo);
        
        output += (Li * Fr * d);
    }
    
    return output;
}


float4 PBR()
{
    float3 output = g_Emission;
    float w = materials[g_MaterialIndex].Albedo.w;
    
    output += CalculatePointLights();
    output += CalculateSpotLights();
    output += CalculateDirectionalLights();

    float3 ambient = lightingData.Ambient.xyz * g_Albedo * g_AmbientOcclusion;
    output += ambient;
    
    output /= (output + float3(1.0, 1.0, 1.0));
    output = pow(output, float3(1 / 2.2, 1 / 2.2, 1 / 2.2));
    
    return float4(output, w);
}

float4 Main(in VSInput IN) : SV_Target
{
    g_InputPosition = IN.Position;
    g_InstanceIndex = IN.InstanceID;
    g_MaterialIndex = drawDescriptorIndices[g_InstanceIndex].MaterialIndex;
    g_View = normalize(camera.Position.xyz - IN.Position);
    g_Albedo = materials[g_MaterialIndex].Albedo.xyz;
    g_Emission = materials[g_MaterialIndex].Emission.xyz;
    g_Normal = normalize(IN.Normal);
    g_Metallic = materials[g_MaterialIndex].Metallic;
    g_Roughness = materials[g_MaterialIndex].Roughness;
    g_AmbientOcclusion = materials[g_MaterialIndex].AmbientOcclusion;
    
    if ((materials[g_MaterialIndex].SamplerBits & g_AlbedoMapMask)  != 0)
        g_Albedo = smpAlbedo.Sample(smpStateAlbedo, IN.AlbedoTexCoord).xyz;
    if ((materials[g_MaterialIndex].SamplerBits & g_EmissionMapMask) != 0)
        g_Emission = smpEmission.Sample(smpStateEmission, IN.EmissionTexCoord).xyz;
    if ((materials[g_MaterialIndex].SamplerBits & g_NormalMapMask)  != 0)
        g_Normal = normalize(smpNormal.Sample(smpStateNormal, IN.NormalTexCoord).rgb);
    if ((materials[g_MaterialIndex].SamplerBits & g_MetallicMapMask) != 0)
        g_Metallic = smpMetallic.Sample(smpStateMetallic, IN.MetallicTexCoord).x;
    if ((materials[g_MaterialIndex].SamplerBits & g_RoughnessMapMask) != 0)
        g_Roughness = smpRoughness.Sample(smpStateRoughness, IN.RoughnessTexCoord).x;
    if ((materials[g_MaterialIndex].SamplerBits & g_AmbientOcclusionMapMask) != 0)
        g_AmbientOcclusion = smpAmbientOcclusion.Sample(smpStateAmbientOcclusion, IN.AmbientOcclusionTexCoord).x;
    
    g_F0 = lerp(float3(0.04, 0.04, 0.04), g_Albedo, float3(g_Metallic, g_Metallic, g_Metallic));
    g_Alpha = SQ(g_Roughness);
    g_K = g_Alpha / 2.0;
    
    return PBR();
}