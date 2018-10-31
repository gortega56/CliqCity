#include "Lighting.hlsli"
#include "Sobel.hlsli"

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
    int4 textureIndices; // x - diffuse, y - bump z - dissolve w - spec
    float4 p2[10];
};

cbuffer Constants : register(b0)
{
    float4x4 view;
    float4x4 proj;
    float4x4 inverseView;
    float4x4 inverseProj;
}

cbuffer LightConstants : register(b1)
{
    float4x4 lightView;
    float4x4 lightProj;
    float4x4 lightInverseView;
    float4x4 lightInverseProj;
}

cbuffer Light : register(b2)
{
    float4 Color;
    float4 Direction;
}

ConstantBuffer<Material> materials[] : register(b3);

Texture2D textures[] : register(t0, space0);
Texture2D shadow : register(t0, space1);

SamplerState default_sampler : register(s0);
SamplerComparisonState shadow_sampler : register(s1);

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

    matrix T =
    {
        0.5f, 0.0f, 0.0f, 0.0f,
        0.0f,-0.5f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 1.0f
    };

    float4x4 VPT = mul(mul(lightView, lightProj), T);
    float4 Lp = mul(float4(input.worldPos, 1), VPT);
    float Sf = Shadow_Factor(shadow, shadow_sampler, Lp.xy, Lp.z);

    float3 N = normalize(input.norm);
    float3 L = -Direction.xyz;
    float3 Lc = Color.xyz;
    float Ia = 0.5f;
    float Id = 8.5f;
    float Is = 25.0f;
    float r = 0.0f;
    int iMaterial = input.mat;
    if (iMaterial != -1)
    {
        float3 ambient = float3(0, 0, 0);
        float3 diffuse = float3(0, 0, 0);
        float3 specular = float3(0, 0, 0);

        int iBump = materials[iMaterial].textureIndices.y;
        if (iBump != -1)
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

            N = SobelFilter(textures[iBump], default_sampler, input.uv, 1.0f);
            //N = normalize(mul(N, tbn));
        }
        
        int iDiffuse = materials[iMaterial].textureIndices.x;
        if (iDiffuse != -1)
        {
            float3 color = textures[iDiffuse].Sample(default_sampler, input.uv).xyz;
            color = SRGB_to_Linear(color);

            // Unlit
            ambient = color * materials[iMaterial].ambient * Ia;

            diffuse = color * materials[iMaterial].diffuse;
            diffuse = Blinn_Phong_Diffuse(diffuse, N, L, Lc, Id);
        }

        int iMask = materials[iMaterial].textureIndices.z;
        if (iMask != -1)
        {
            float4 m = textures[iMask].Sample(default_sampler, input.uv);
            if ((m.x + m.y + m.z) == 0) clip(-1);
        }

        int iSpec = materials[iMaterial].textureIndices.w;
        if (iSpec != -1)
        {
            float3 eye = float3(inverseView[3][0], inverseView[3][1], inverseView[3][2]);
            float3 V = normalize(eye - input.worldPos);
            float3 H = normalize(L + V);
            float specExp = materials[iMaterial].specularExponent;

            specular = materials[iMaterial].specular * textures[iSpec].Sample(default_sampler, input.uv).xyz;
            specular = Blinn_Phong_Spec(specular, N, H, Lc, specExp, Is);
        }


        float3 shadow = (ambient) * max(0.2f, Sf);
        float3 color = (ambient + diffuse + specular) * Sf;
        output.xyz = lerp(shadow, color, Sf);



        //output.xyz = Reinhard_Simple(output.xyz, 1.5f);
        output.xyz *= 0.9f;
        output.xyz = Reinhard_Luma(output.xyz);
        //output.xyz = ACESFilm(output.xyz);

        output.xyz = Linear_to_SRGB(output.xyz);
        output.xyz = N;
    }

    return output;
}