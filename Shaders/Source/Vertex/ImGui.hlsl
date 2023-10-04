/* Vertex shader */
struct VSInput
{
    [[vk::location(0)]] float2 Position : POSITION0;
    [[vk::location(1)]] float2 TexCoord : TEXCOORD0;
    [[vk::location(2)]] float4 Color : COLOR0;
};

struct VSOutput
{
    float4 Position : SV_POSITION;
    [[vk::location(0)]] float2 TexCoord : TEXCOORD0;
    [[vk::location(1)]] float4 Color : COLOR0;
};

struct UBO
{
    float2 Size;
};

cbuffer ubo : register(b0, space0)
{ 
    UBO ubo;
}

VSOutput main(in VSInput IN)
{
    VSOutput OUT;
    OUT.Position = float4(IN.Position.x * 2 / ubo.Size.x - 1, -IN.Position.y * 2 / ubo.Size.y + 1.f, 0, 1);
    OUT.TexCoord = IN.TexCoord;
    OUT.Color = IN.Color;
    
    return OUT;
}