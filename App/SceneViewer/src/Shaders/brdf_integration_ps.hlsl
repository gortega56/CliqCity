#include "Lighting.hlsli"

struct ps_input
{
    float4 Pos : SV_POSITION;
    float2 UV : TEXCOORD0;
};

cbuffer Constants : register(b1)
{
    float4 Roughness;   // unused here
    uint LightingMode;
};

float2 main(ps_input input) : SV_TARGET
{
    float NoV = input.UV.x;
    float roughness = input.UV.y;

    float2 dfg = float2(0, 0);

    if (LightingMode == 1) dfg = DFG_Blinn_Phong(NoV, roughness);
    else if (LightingMode == 2) dfg = DFG_Beckmann(NoV, roughness);
    else if (LightingMode == 3) dfg = DFG_GGX(NoV, roughness);

    return dfg;
}