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
    float3 eye = float3(InverseView[3][0], InverseView[3][1], InverseView[3][2]);
    
    matrix vp = mul(View, Projection);

    VS_Output output;
    output.Position = mul(float4(position + eye, 1), vp);
    output.Position.z = output.Position.w;
    output.UV = position;
    return output;
}