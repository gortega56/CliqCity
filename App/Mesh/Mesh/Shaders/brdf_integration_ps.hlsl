#include "Lighting.hlsli"

struct ps_input
{
    float4 Pos : SV_POSITION;
    float2 UV : TEXCOORD0;
};

float2 main(ps_input input) : SV_TARGET
{
    float NoV = input.UV.x;
    float roughness = input.UV.y;
    return environment_brdf(NoV, roughness);
}