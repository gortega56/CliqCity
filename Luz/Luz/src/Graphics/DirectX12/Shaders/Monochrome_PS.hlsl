#include "Lighting.hlsli"

cbuffer SceneLighting : register(b0)
{
    float4 Color;
    float4 Direction;
}

struct PS_Input
{
    float4 position : SV_POSITION;
    float3 normal   : NORMAL;
};

float4 main(PS_Input input) : SV_TARGET
{
    float4 output = float4(0.0f, 0.0f, 0.0f, 1.0f);

    float3 N = normalize(input.normal);
    float3 L = Direction.xyz;
    float3 Lc = Color.xyz;
    float  Id = 1.0f;
    
    float3 diffuse = float3(0.5f, 0.5f, 0.5f);

    output.xyz = Blinn_Phong_Diffuse(diffuse, N, -L, Lc, Id);

    return output;
}