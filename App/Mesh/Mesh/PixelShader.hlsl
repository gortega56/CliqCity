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
    uint LightingMode;
    uint AmbientEnabled;
    uint DiffuseEnabled;
    uint SpecEnabled;
    uint BumpEnabled;
    uint ShadowEnabled;
    uint MicrofacetEnabled;
    uint MaskingEnabled;
    uint FresnelEnabled;
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

struct ShadingFrame
{
    float3 P;
    float3 E;
    float3 V;
    float3 L;
    float3 H;
    float3 T;
    float3 B;
    float3 N;
    float3 Lc;
    float2 UV;
    float Ia;
    float Id;
    float Is;
    int iMaterial;
};

float4 Shade_Blinn_Phong(ShadingFrame frame)
{
    float4 output = float4(1, 0, 1, 1); // magenta


    int iMaterial = frame.iMaterial;
    if (iMaterial != -1)
    {
        int iMask = materials[iMaterial].textureIndices.z;
        if (iMask != -1)
        {
            float m = textures[iMask].Sample(default_sampler, frame.UV).r;
            clip(m - 0.001f);
        }

        float3 ambient = float3(0, 0, 0);
        float3 diffuse = float3(0, 0, 0);
        float3 specular = float3(0, 0, 0);
        float3 N = frame.N;

        int iBump = materials[iMaterial].textureIndices.y;
        bool bumpEnabled = BumpEnabled && (iBump != -1);
        if (bumpEnabled)
        {
            float3x3 tbn =
            {
                frame.T.x, frame.T.y, frame.T.z,
                frame.B.x, frame.B.y, frame.B.z,
                frame.N.x, frame.N.y, frame.N.z
            };

            N = SobelFilter(textures[iBump], bump_sampler, frame.UV, 1.0f);
            N = normalize(mul(N, tbn));
        }

        int iDiffuse = materials[iMaterial].textureIndices.x;
        bool diffuseEnabled = DiffuseEnabled && (iDiffuse != -1);
        if (diffuseEnabled)
        {
            float3 color = textures[iDiffuse].Sample(default_sampler, frame.UV).xyz;
            color = SRGB_to_Linear(color);

            // Unlit
            if (AmbientEnabled)
            {
                ambient = color * materials[iMaterial].ambient * frame.Ia;
            }

            diffuse = color * materials[iMaterial].diffuse;
            diffuse = Blinn_Phong_Diffuse(diffuse, N, frame.L, frame.Lc, frame.Id);
        }

        int iSpec = materials[iMaterial].textureIndices.w;
        bool specEnabled = SpecEnabled && (iSpec != -1);
        if (specEnabled)
        {
            float specExp = materials[iMaterial].specularExponent;
            specular = materials[iMaterial].specular * textures[iSpec].Sample(default_sampler, frame.UV).r;
            specular = Blinn_Phong_Spec(specular, N, frame.H, frame.Lc, specExp, frame.Is);
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
            float4 Lp = mul(float4(frame.P, 1), VPT);
            Sf = Shadow_Factor(shadow, shadow_sampler, Lp.xy, Lp.z);
        }

        output.xyz = lerp(ambient, (ambient + diffuse + specular), Sf);
    }

    return output;
}

float4 Shade_GGX(ShadingFrame frame)
{
    float4 output = float4(1, 0, 1, 1); // magenta

    float3 N = frame.N;

    int iMaterial = frame.iMaterial;
    if (iMaterial != -1)
    {
        int iMask = materials[iMaterial].textureIndices.z;
        if (iMask != -1)
        {
            float m = textures[iMask].Sample(default_sampler, frame.UV).r;
            clip(m - 0.001f);
        }

        float3 ambient = float3(0, 0, 0);
        float3 diffuse = float3(0, 0, 0);
        float3 specular = float3(0, 0, 0);
        float3 N = frame.N;

        int iBump = materials[iMaterial].textureIndices.y;
        bool bumpEnabled = BumpEnabled && (iBump != -1);
        if (bumpEnabled)
        {
            float3x3 tbn =
            {
                frame.T.x, frame.T.y, frame.T.z,
                frame.B.x, frame.B.y, frame.B.z,
                frame.N.x, frame.N.y, frame.N.z
            };

            N = SobelFilter(textures[iBump], bump_sampler, frame.UV, 1.0f);
            N = normalize(mul(N, tbn));
        }

        float NoV = abs(dot(N, frame.V)) + 1e-5;
        float NoL = saturate(dot(N, frame.L));
        float NoH = saturate(dot(N, frame.H));
        float VoH = saturate(dot(frame.H, frame.V));

        int iDiffuse = materials[iMaterial].textureIndices.x;
        bool diffuseEnabled = DiffuseEnabled && (iDiffuse != -1);
        if (diffuseEnabled)
        {
            float3 color = textures[iDiffuse].Sample(default_sampler, frame.UV).xyz;
            color = SRGB_to_Linear(color);

            // Unlit
            if (AmbientEnabled)
            {
                ambient = color * materials[iMaterial].ambient * frame.Ia;
            }

            diffuse = color * materials[iMaterial].diffuse * Fd_Lambert() * NoL * frame.Lc * frame.Id;
        }

        int iSpec = materials[iMaterial].textureIndices.w;
        bool specEnabled = SpecEnabled && (iSpec != -1);
        if (specEnabled)
        {
            float F0 = 0.035f;
            float F90 = 1.0f;

            float3 color = materials[iMaterial].specular;
            if (!any(color))
            {
                color = float3(F0, F0, F0); 
            }

            float3 tex = textures[iSpec].Sample(default_sampler, frame.UV).xyz;
            if (!all(tex))
            {
                tex = tex.xxx;
            }

            color *= tex;

            float shininess = materials[iMaterial].specularExponent;
            float roughness = sqrt(2.0 / (shininess + 2.0));
            roughness = roughness * roughness;

            float F = 1.0;
            if (FresnelEnabled)
            {
                F = F_Schlick(F0, F90, NoH);
            }

            float D = 1.0f;
            if (MicrofacetEnabled)
            {
                D = D_GGX(NoH, roughness);
            }

            float G = 1.0f;
            if (MaskingEnabled)
            {
                G = G_Smith_GGX(NoV, NoL, roughness);
            }

            specular = color * D * G * F * NoL * frame.Lc * frame.Is;
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
            float4 Lp = mul(float4(frame.P, 1), VPT);
            Sf = Shadow_Factor(shadow, shadow_sampler, Lp.xy, Lp.z);
        }

        output.xyz = lerp(ambient, (ambient + diffuse + specular), Sf);
    }

    return output;
}

float4 main(PS_Input input) : SV_TARGET
{
    ShadingFrame frame;
    frame.P = input.worldPos;
    frame.E = float3(inverseView[3][0], inverseView[3][1], inverseView[3][2]);
    frame.V = normalize(frame.E - frame.P);
    frame.L = normalize(-Direction.xyz);
    frame.H = normalize(frame.L + frame.V);
    frame.T = normalize(input.tangent);
    frame.B = normalize(input.bitangent);
    frame.N = normalize(input.norm);
    frame.Lc = Color.xyz;
    frame.UV = input.uv;
    frame.Ia = Intensity.x;
    frame.Id = Intensity.y;
    frame.Is = Intensity.z;
    frame.iMaterial = input.mat;

    float4 output;
    if (LightingMode == 0)
    {
        output = Shade_Blinn_Phong(frame);
    }
    else
    {
        output = Shade_GGX(frame);
    }

    output.xyz *= Exposure;
    output.xyz = Reinhard_Luma(output.xyz);
    output.xyz = Linear_to_SRGB(output.xyz);

    return output;
}