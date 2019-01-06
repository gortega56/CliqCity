struct VS_Output
{
    float4 Position : SV_POSITION;
    float3 UV : TEXCOORD;
};

cbuffer Constants : register(b0)
{
    float4x4 View;
    float4x4 Projection;
    float4x4 InverseView;
    float4x4 InverseProj;
}

VS_Output main(float3 position : POSITION)
{
    matrix vp = mul(View, Projection);

    VS_Output output;
    output.Position = mul(float4(position, 1), vp);
    output.UV = position;
    return output;
}