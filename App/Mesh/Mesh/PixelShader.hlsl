Texture2D diffuseTexture : register(t0);
Texture2D normalTexture : register(t1);
SamplerState samp : register(s0);

struct VS_OUTPUT
{
    float4 pos: SV_POSITION;
    float3 tangent : TANGENT;
    float3 bitangent : BINORMAL;
    float3 norm : NORMAL;
    float2 uv: TEXCOORD;
};

float4 main(VS_OUTPUT input) : SV_TARGET
{
    float3 t = normalize(input.tangent);
    float3 b = normalize(input.bitangent);
    float3 n = normalize(input.norm);

    float3x3 tbn = 
    {
        t.x, t.y, t.z,
        b.x, b.y, b.z,
        n.x, n.y, n.z
    };
   
    float3 normTex = normalTexture.Sample(samp, input.uv).xyz * 2.0f - 1.0f;
    n = mul(normTex, tbn);

    float3 l = float3(0.0f, 0.5f, -0.5f);

    float4 color = float4(0.8, 0.8f, 0.8f, 1.0f);
    float4 diff = diffuseTexture.Sample(samp, input.uv);

    float nDotL = dot(l, n);

    return color * diff * saturate(nDotL);
}