cbuffer MaterialConstants : register(b1)
{
    float specularExponent;
    float transparency;
    float opticalDensity;
    float dissolve;
    float3 specular;
    float3 transmissionFilter;
    float3 ambient;
    float3 diffuse;
    float3 emissive;
}

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
   
    float3 ns = normalTexture.Sample(samp, input.uv).xyz;
    float3 tns = ns * 2.0f - 1.0f;
    n = mul(tns, tbn);

    float3 l = float3(0.0f, -0.5f, 0.5f);

    float4 color = float4(0.8, 0.8f, 0.8f, 1.0f);
    
    float4 diff = diffuseTexture.Sample(samp, input.uv);

    float nDotL = dot(n, -l);

    return color * diff * saturate(nDotL);
}