#include "Lighting.hlsli"

struct PS_Input
{
    float4 Position : SV_POSITION;
    float3 UV : TEXCOORD;
};

cbuffer Constants : register(b1)
{
    float4 Roughness;
    uint LightingMode;
};

TextureCube cube_texture : register(t0);

SamplerState cube_sampler : register(s0);

float4 main(PS_Input input) : SV_TARGET
{
    float3 ld = float3(0,0,0);
    if (LightingMode == 1) ld = LD_Blinn_Phong(cube_texture, cube_sampler, normalize(input.UV), Roughness.x);
    else if (LightingMode == 2) ld = LD_Beckmann(cube_texture, cube_sampler, normalize(input.UV), Roughness.x);
    else if (LightingMode == 3) ld = LD_GGX(cube_texture, cube_sampler, normalize(input.UV), Roughness.x);

    return float4(ld, 1);

}