struct Material
{
    float3 specular;
    float specularExponent;
    float3 transmissionFilter;
    float transparency;
    float3 ambient;
    float opticalDensity;
    float3 diffuse;
    float dissolve;
    float3 emissive;
    float p0;
    int3 textureIndices; // x - diffuse, y - normal z - dissolve
    float p1;
    float4 p2[10];
};

ConstantBuffer<Material> materials[] : register(b1);

Texture2D textures[] : register(t0);
SamplerState samp : register(s0);

struct VS_OUTPUT
{
    float4 pos: SV_POSITION;
    float3 tangent : TANGENT;
    float3 bitangent : BINORMAL;
    float3 norm : NORMAL;
    float2 uv : TEXCOORD0;
    int mat : BLENDINDICES;
};

float4 main(VS_OUTPUT input) : SV_TARGET
{
    float3 l = float3(0.0f, -0.5f, 0.5f);

    float4 color = float4(0.8, 0.8f, 0.8f, 1.0f);

    float4 diff = float4(1.0f, 0.0f, 1.0f, 1.0f);
    float albedo = dot(input.norm, -l);

    int matIndex = input.mat;
    if (matIndex != -1)
    {
        int diffIndex = materials[matIndex].textureIndices.x;
        int normIndex = materials[matIndex].textureIndices.y;
        int mask = materials[matIndex].textureIndices.z;

        diff = textures[diffIndex].Sample(samp, input.uv);


        if (normIndex != -1)
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

            float3 ns = textures[normIndex].Sample(samp, input.uv).xyz;

            float3 tns = ns * 2.0f - 1.0f;
            n = mul(tns, tbn);

            albedo = dot(n, -l);
        }
    }

    return color * diff * saturate(albedo);
}