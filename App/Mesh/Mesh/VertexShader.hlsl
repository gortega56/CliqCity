struct VS_INPUT
{
    float4 tangent : TANGENT;
    float3 pos : POSITION;
    float3 norm : NORMAL;
    float3 uv : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float3 tangent : TANGENT;
    float3 bitangent : BINORMAL;
    float3 norm : NORMAL;
    float2 uv : TEXCOORD0;
    int mat : BLENDINDICES;
};

cbuffer Constants : register(b0)
{
    float4x4 view;
    float4x4 proj;
}

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    matrix wvp = mul(view, proj);
    output.pos = mul(float4(input.pos, 1.0f), wvp);
    output.tangent = input.tangent.xyz;
    output.bitangent = cross(input.norm, input.tangent.xyz) * input.tangent.w;
    output.norm = input.norm;
    output.uv = input.uv.xy;
    output.mat = (int)input.uv.z;
    return output;
}