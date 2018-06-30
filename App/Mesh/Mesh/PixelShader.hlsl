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
    int4 textureIndices; // x - diffuse, y - normal z - dissolve
    float4 p2[10];
};

cbuffer Constants : register(b0)
{
    float4x4 view;
    float4x4 proj;
    float4x4 inverseView;
    float4x4 inverseProj;
}

ConstantBuffer<Material> materials[] : register(b1);

Texture2D textures[] : register(t0);
SamplerState samp : register(s0);
SamplerState clampSamp : register(s1);


struct VS_OUTPUT
{
    float4 pos: SV_POSITION;
    float3 worldPos : POSITIONT;
    float3 tangent : TANGENT;
    float3 bitangent : BINORMAL;
    float3 norm : NORMAL;
    float2 uv : TEXCOORD0;
    int mat : BLENDINDICES;
};

float4 main(VS_OUTPUT input) : SV_TARGET
{
    float4 output = float4(1, 0, 1, 1); // magenta

    float3 N = normalize(input.norm);
    float3 L = float3(0.0f, -0.5f, 0.5f);
    float3 Lc = float3(0.8, 0.8f, 0.8f);
    float Ia = 0.1f;
    float Id = 1.0f;
    float Is = 1.0f;

    int matIndex = input.mat;
    if (matIndex != -1)
    {
        float3 ambient =  float3(0, 0, 0);
        float3 diffuse =  float3(0, 0, 0);
        float3 specular = float3(0, 0, 0);

        ambient = materials[matIndex].ambient * Lc * Ia;

        int diffIndex = materials[matIndex].textureIndices.x;
        int normIndex = materials[matIndex].textureIndices.y;
        int maskIndex = materials[matIndex].textureIndices.z;
        int specIndex = materials[matIndex].textureIndices.w;

        if (diffIndex != -1)
        {
            diffuse = materials[matIndex].diffuse * textures[diffIndex].Sample(samp, input.uv).xyz;
        }

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

            N = textures[normIndex].Sample(samp, input.uv).xyz;
            N = N * 2.0f - 1.0f;
            N = normalize(mul(N, tbn));
        }

        if (maskIndex != -1)
        {
            float4 m = textures[maskIndex].Sample(samp, input.uv);
            if ((m.x + m.y + m.z) == 0) clip(-1);
        }

        if (specIndex != -1)
        {
            specular = textures[specIndex].Sample(samp, input.uv).xyz;
            float specExp = materials[matIndex].specularExponent;
            float3 eye = float3(inverseView[3][0], inverseView[3][1], inverseView[3][2]);
            float3 V = normalize(eye - input.worldPos);
            float3 H = normalize(-L + V);
            specular = specular * pow(saturate(dot(N, H)), specExp) * Lc * Is;
        }

        diffuse = diffuse * saturate(dot(N, -L)) * Lc * Id;

        output.xyz = ambient + diffuse + specular;
    }

    return output;
}