Texture2D t1 : register(t0);
SamplerState s1 : register(s0);

struct VS_OUTPUT
{
    float4 pos: SV_POSITION;
    float3 norm : NORMAL;
    float2 uv: TEXCOORD;
};

float4 main(VS_OUTPUT input) : SV_TARGET
{
    float3 l = float3(0.0f, 0.5f, -0.5f);
    float3 n = normalize(input.norm);

    float4 color = float4(0.8, 0.8f, 0.8f, 1.0f);
    float4 diff = t1.Sample(s1, input.uv);

    float nDotL = dot(l, n);

    return color * diff * saturate(nDotL);
}