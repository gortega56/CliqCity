struct PS_Input
{
    float4 Position : SV_POSITION;
    float3 UV : TEXCOORD;
};

TextureCube cube_texture : register(t0, space2);

SamplerState cube_sampler : register(s0);

float4 main(PS_Input input) : SV_TARGET
{
    return cube_texture.Sample(cube_sampler, normalize(input.UV));
}