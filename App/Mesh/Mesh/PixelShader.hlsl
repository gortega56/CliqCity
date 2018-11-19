#include "Lighting.hlsli"
#include "Sobel.hlsli"

cbuffer CameraConstants : register(b0)
{
    float4x4 view;
    float4x4 proj;
    float4x4 inverseView;
    float4x4 inverseProj;
}

cbuffer ShadowConstants : register(b1)
{
    float4x4 shadowView;
    float4x4 shadowProj;
    float4x4 shadowInverseView;
    float4x4 shadowInverseProj;
}

cbuffer LightingConstants : register(b2)
{
    float4 Color;
    float4 Direction;
    float4 Intensity; // ambient, diffuse, spec
    float Exposure;
    uint AmbientEnabled;
    uint DiffuseEnabled;
    uint SpecEnabled;
    uint BumpEnabled;
    uint ShadowEnabled;
}

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

ConstantBuffer<Material> materials[] : register(b3);

Texture2D textures[] : register(t0, space0);

Texture2D shadow : register(t0, space1);

SamplerState default_sampler : register(s0);

SamplerState bump_sampler : register(s2);

SamplerComparisonState shadow_sampler : register(s1);

struct PS_Input
{
    float4 pos: SV_POSITION;
    float3 worldPos : POSITIONT;
    float3 tangent : TANGENT;
    float3 bitangent : BINORMAL;
    float3 norm : NORMAL;
    float2 uv : TEXCOORD0;
    int mat : BLENDINDICES;
};

float4 main(PS_Input input) : SV_TARGET
{
    float4 output = float4(1, 0, 1, 1); // magenta

    float3 N = normalize(input.norm);
    float3 L = normalize(-Direction.xyz);
    float3 Lc = Color.xyz;
    float Ia = Intensity.x;
    float Id = Intensity.y;
    float Is = Intensity.z;
    
    int iMaterial = input.mat;
    if (iMaterial != -1)
    {
        int iMask = materials[iMaterial].textureIndices.z;
        if (iMask != -1)
        {
            float m = textures[iMask].Sample(default_sampler, input.uv).r;
            clip(m - 0.001f);
        }

        float3 ambient = float3(0, 0, 0);
        float3 diffuse = float3(0, 0, 0);
        float3 specular = float3(0, 0, 0);

        int iBump = materials[iMaterial].textureIndices.y;
        bool bumpEnabled = BumpEnabled && (iBump != -1);
        if (bumpEnabled)
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

            N = SobelFilter(textures[iBump], bump_sampler, input.uv, 1.0f);
            N = normalize(mul(N, tbn));
        }

        int iDiffuse = materials[iMaterial].textureIndices.x;
        bool diffuseEnabled = DiffuseEnabled && (iDiffuse != -1);
        if (diffuseEnabled)
        {
            float3 color = textures[iDiffuse].Sample(default_sampler, input.uv).xyz;
            color = SRGB_to_Linear(color);

            // Unlit
            if (AmbientEnabled)
            {
                ambient = color * materials[iMaterial].ambient * Ia;
            }

            diffuse = color * materials[iMaterial].diffuse;
            diffuse = Blinn_Phong_Diffuse(diffuse, N, L, Lc, Id);
        }

        int iSpec = materials[iMaterial].textureIndices.w;
        bool specEnabled = SpecEnabled && (iSpec != -1);
        if (specEnabled)
        {
            float3 E = float3(inverseView[3][0], inverseView[3][1], inverseView[3][2]);
            float3 V = normalize(E - input.worldPos);
            float3 H = normalize(L + V);
            float specExp = materials[iMaterial].specularExponent;

            specular = materials[iMaterial].specular * textures[iSpec].Sample(default_sampler, input.uv).r;
            specular = Blinn_Phong_Spec(specular, N, H, Lc, specExp, Is);
        }

        float Sf = 1.0f;
        if (ShadowEnabled)
        {
            matrix T =
            {
                0.5f, 0.0f, 0.0f, 0.0f,
                0.0f,-0.5f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.5f, 0.5f, 0.0f, 1.0f
            };

            float4x4 VPT = mul(mul(shadowView, shadowProj), T);
            float4 Lp = mul(float4(input.worldPos, 1), VPT);
            Sf = Shadow_Factor(shadow, shadow_sampler, Lp.xy, Lp.z);
        }

        float3 shadow = (ambient) * max(0.2f, Sf);
        float3 color = (ambient + diffuse + specular) * Sf;
        output.xyz = lerp(shadow, color, Sf);

        output.xyz *= Exposure;
        output.xyz = Reinhard_Luma(output.xyz);
        output.xyz = Linear_to_SRGB(output.xyz);
    }

    return output;
}