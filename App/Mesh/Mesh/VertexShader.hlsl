struct VS_INPUT
{
    float3 tangent : TANGENT;
    float3 pos : POSITION;
    float3 norm : NORMAL;
    float2 uv : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float3 tangent : TANGENT;
    float3 bitangent : BINORMAL;
    float3 norm : NORMAL;
    float2 uv : TEXCOORD;
};

cbuffer Constants : register(b0)
{
    float4x4 model;
    float4x4 view;
    float4x4 proj;
}

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    matrix wvp = mul(mul(model, view), proj);
    output.pos = mul(float4(input.pos, 1.0f), wvp);
    output.tangent = mul(input.tangent, (float3x3)model);
    output.bitangent = mul(cross(input.norm, input.tangent), (float3x3)model);
    output.norm = mul(input.norm, (float3x3)model);
    output.uv = input.uv;
    return output;
}