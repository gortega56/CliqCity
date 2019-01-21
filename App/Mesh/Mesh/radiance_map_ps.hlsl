#include "Lighting.hlsli"

struct PS_Input
{
    float4 Position : SV_POSITION;
    float3 UV : TEXCOORD;
};

cbuffer Constants : register(b1)
{
    float4 s_roughness;
};

TextureCube cube_texture : register(t0, space2);

SamplerState cube_sampler : register(s0);

float4 main(PS_Input input) : SV_TARGET
{
    float3 radiance = Environment_Radiance(cube_texture, cube_sampler, normalize(input.UV), s_roughness.x);

    return float4(radiance, 1);

}