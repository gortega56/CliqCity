struct VS_INPUT
{
    float4 color : COLOR;
    float3 pos : POSITION;
};

struct VS_OUTPUT
{
    float4 color : COLOR;
    float4 pos : SV_POSITION;
};

cbuffer ConstantBuffer : register(b0)
{
    float4x4 model;
    float4x4 view;
    float4x4 proj;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    matrix wvp = mul(mul(model, view), proj);
    output.pos = mul(float4(input.pos, 1.0f), wvp);
    output.color = input.color;
    return output;
}