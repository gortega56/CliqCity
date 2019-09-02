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
    float4 Intensity; // ambient, diffuse, spec, lx
    float Exposure;
    uint LightingMode;
    uint BumpMode;
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
    float _unused0;
	int iAmbient;
    int iDiffuse;
    int iSpec;
    int iBump;
    int iNormal;
	int iAlpha;
	int iEmissive;
	int iShininess;
	int iReflection;
    int iMetal;
    int iRough;
    float _unused1[33];
};

ConstantBuffer<Material> materials[] : register(b3);

Texture2D textures[] : register(t0, space0);
Texture2D shadow : register(t0, space1);
Texture2D environment_brdf_texture : register(t0, space2);
TextureCube environment_texture : register(t0, space3);

SamplerState linear_wrap_sampler : register(s0);
SamplerState aniso_wrap_sampler : register(s1);
SamplerComparisonState point_border_sampler : register(s2);

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
    float Lx;
    int iMaterial;
};

float3 GetNormal(ShadingFrame frame, int iBump, int iNormal, uint mode)
{
    float3x3 tbn =
    {
        frame.T.x, frame.T.y, frame.T.z,
        frame.B.x, frame.B.y, frame.B.z,
        frame.N.x, frame.N.y, frame.N.z
    };

    float3 N = frame.N;

    if (mode == 0 && iBump != -1)
    {
        N = SobelFilter(textures[iBump], linear_wrap_sampler, frame.UV, 1.0f);
        N = normalize(mul(N, tbn));
    }
    else if (mode == 1 && iNormal != -1)
    {
        N = textures[iNormal].Sample(linear_wrap_sampler, frame.UV).xyz;
        N = N * 2.0 - 1.0;
        N = normalize(mul(N, tbn));
    }

    return N;
}

float GetShadow(ShadingFrame frame)
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
    return Shadow_Factor(shadow, point_border_sampler, Lp.xy, Lp.z);
}

float4 Shade_Default(ShadingFrame frame)
{
    float4 output = float4(1, 0, 1, 1); // magenta

    int iMaterial = frame.iMaterial;
    if (iMaterial != -1)
    {
        float4 base = float4(0, 0, 0, 0);
        float3 ambient = float3(0, 0, 0);
        float3 diffuse = float3(1, 0, 1);
        float3 specular = float3(0, 0, 0);
        float3 N = frame.N;

		int iAlpha = materials[iMaterial].iAlpha;
		if (iAlpha != -1)
		{
			float alpha = textures[iAlpha].Sample(linear_wrap_sampler, frame.UV).r;
			clip(alpha - 0.001f);
		}

        int iDiffuse = materials[iMaterial].iDiffuse;
        if (iDiffuse != -1)
        {
            base = textures[iDiffuse].Sample(linear_wrap_sampler, frame.UV);
            clip(base.a - 0.001f);
            base.xyz = Gamma_to_Linear(base.xyz);
        }

        // Unlit
        if (AmbientEnabled)
        {
            ambient = base.xyz * materials[iMaterial].ambient * frame.Ia;
        }

        int iBump = materials[iMaterial].iBump;
        int iNormal = materials[iMaterial].iNormal;
        bool bumpEnabled = BumpEnabled;
        if (bumpEnabled)
        {
            N = GetNormal(frame, iBump, iNormal, BumpMode);
        }

        bool diffuseEnabled = DiffuseEnabled && (iDiffuse != -1);
        if (diffuseEnabled)
        {
            diffuse = base.xyz * materials[iMaterial].diffuse;
            diffuse = Blinn_Phong_Diffuse(diffuse, N, frame.L, frame.Lc, frame.Id);
        }

        int iSpec = materials[iMaterial].iSpec;
        bool specEnabled = SpecEnabled && (iSpec != -1);
        if (specEnabled)
        {
            specular = textures[iSpec].Sample(linear_wrap_sampler, frame.UV).xyz;
            if (!all(specular))
            {
                specular = specular.xxx;
            }

            specular *= materials[iMaterial].specular;

            float specExp = materials[iMaterial].specularExponent;
            specular = Blinn_Phong_Spec(specular, N, frame.H, frame.Lc, specExp, frame.Is);
        }

        float Sf = 1.0f;
        if (ShadowEnabled)
        {
            Sf = GetShadow(frame);
        }

        output.xyz = lerp(ambient, (ambient + diffuse + specular), Sf);
    }
    
    return output;
}

float4 Shade_Blinn_Phong(ShadingFrame frame)
{
    float4 output = float4(1, 0, 1, 1); // magenta

    int iMaterial = frame.iMaterial;
    if (iMaterial != -1)
    {
        float4 base = float4(0, 0, 0, 0);
        float3 ambient = float3(0, 0, 0);
        float3 diffuse = float3(0, 0, 0);
        float3 specular = float3(0, 0, 0);
        float3 N = frame.N;
        float metallic = 0;

		int iAlpha = materials[iMaterial].iAlpha;
		if (iAlpha != -1)
		{
			float alpha = textures[iAlpha].Sample(linear_wrap_sampler, frame.UV).r;
			clip(alpha - 0.001f);
		}

        int iDiffuse = materials[iMaterial].iDiffuse;
        if (iDiffuse != -1)
        {
            base = textures[iDiffuse].Sample(linear_wrap_sampler, frame.UV);
            clip(base.a - 0.1f);
            base.xyz = SRGB_to_Linear(base.xyz);
        }

        // Unlit
        if (AmbientEnabled)
        {
            ambient = base.xyz * 0.1f;
        }

        int iBump = materials[iMaterial].iBump;
        int iNormal = materials[iMaterial].iNormal;
        bool bumpEnabled = BumpEnabled;
        if (bumpEnabled)
        {
            N = GetNormal(frame, iBump, iNormal, BumpMode);
        }

        int iMetal = materials[iMaterial].iMetal;
        if (iMetal != -1)
        {
            metallic = textures[iMetal].Sample(linear_wrap_sampler, frame.UV).x;
        }

        float NoV = saturate(dot(N, frame.V));// abs(dot(N, frame.V)) + 1e-5;
        float NoL = saturate(dot(N, frame.L));
        float NoH = saturate(dot(N, frame.H));
        float VoH = saturate(dot(frame.V, frame.H));
        float LoH = saturate(dot(frame.L, frame.H));

        bool diffuseEnabled = DiffuseEnabled && (iDiffuse != -1);
        if (diffuseEnabled)
        {
            diffuse = base.xyz * (1.0 - metallic) * Fd_Lambert();
        }

        int iRough = materials[iMaterial].iRough;
        bool specEnabled = SpecEnabled && (iRough != -1);
        if (specEnabled)
        {
            float3 F90 = float3(1, 1, 1);
            float F0d = 0.04;
            float3 F0 = lerp(F0d.xxx, base.xyz, metallic);

            float3 F = F0;
            if (FresnelEnabled)
            {
                F = F_Schlick(F0, F90, LoH);
            }

            float roughness = textures[iRough].Sample(linear_wrap_sampler, frame.UV).x;
            roughness = pow(8192.0, roughness);
            roughness = 2.0 / (roughness * roughness) - 2.0;
            roughness = roughness * roughness;

            float D = 1.0f;
            if (MicrofacetEnabled)
            {
                D = D_Blinn_Phong(NoH, roughness);
            }

            float G = 1.0f;
            if (MaskingEnabled)
            {                
                G = G_Smith_Beckmann(NoV, NoL, roughness);
            }

            diffuse = (F90 - F) * diffuse;
            specular = (D * G) * F * PI;
        }

        float Sf = 1.0f;
        if (ShadowEnabled)
        {
            Sf = GetShadow(frame);
        }

        float illuminance = NoL * frame.Lx;
        float3 unlit = ambient * frame.Lc * max(0.1f, Sf);
        float3 lit = unlit + (diffuse + specular) * frame.Lc * illuminance;
        output.xyz = lerp(unlit, lit, Sf);
    }

    return output;
}

float4 Shade_Beckmann(ShadingFrame frame)
{
    float4 output = float4(1, 0, 1, 1); // magenta

    int iMaterial = frame.iMaterial;
    if (iMaterial != -1)
    {
        float4 base = float4(0, 0, 0, 0);
        float3 ambient = float3(0, 0, 0);
        float3 diffuse = float3(0, 0, 0);
        float3 specular = float3(0, 0, 0);
        float3 N = frame.N;
        float metallic = 0;
        float roughness = 0;

        int iAlpha = materials[iMaterial].iAlpha;
        if (iAlpha != -1)
        {
            float alpha = textures[iAlpha].Sample(linear_wrap_sampler, frame.UV).r;
            clip(alpha - 0.001f);
        }

        int iDiffuse = materials[iMaterial].iDiffuse;
        if (iDiffuse != -1)
        {
            base = textures[iDiffuse].Sample(aniso_wrap_sampler, frame.UV);
            clip(base.a - 0.1f);
            base.xyz = Gamma_to_Linear(base.xyz);
        }

        int iBump = materials[iMaterial].iBump;
        int iNormal = materials[iMaterial].iNormal;
        bool bumpEnabled = BumpEnabled;
        if (bumpEnabled)
        {
            N = GetNormal(frame, iBump, iNormal, BumpMode);
        }

        int iMetal = materials[iMaterial].iMetal;
        if (iMetal != -1)
        {
            metallic = textures[iMetal].Sample(linear_wrap_sampler, frame.UV).x;
        }

        int iRough = materials[iMaterial].iRough;
        if (iRough != -1)
        {
            roughness = textures[iRough].Sample(linear_wrap_sampler, frame.UV).x;
        }

        float NoV = saturate(dot(N, frame.V));// abs(dot(N, frame.V)) + 1e-5;
        float NoL = saturate(dot(N, frame.L));
        float NoH = saturate(dot(N, frame.H));
        float VoH = saturate(dot(frame.V, frame.H));
        float LoH = saturate(dot(frame.L, frame.H));

        // Unlit
        if (AmbientEnabled)
        {
            // Indirect diffuse;
            const float environmentMipLevels = 6.0;
            const float irradianceMipLevel = environmentMipLevels - 1.0;
            const float maxRadianceMipLevel = irradianceMipLevel - 1.0;

            float3 irradiance = environment_texture.SampleLevel(linear_wrap_sampler, N, irradianceMipLevel).xyz;
            float3 d = (base.xyz * irradiance);

            // Indirect spec
            float roughE = roughness * roughness;
            float F0d = 0.04;

            float3 F90 = float3(1, 1, 1);
            float3 F0 = lerp(F0d.xxx, base.xyz, metallic);
            float3 F = F_Schlick(F0, F90, NoV, roughE);

            float2 brdf = environment_brdf_texture.Sample(linear_wrap_sampler, float2(NoV, roughness)).xy;
            float3 DFG = F0 * brdf.x + brdf.y;

            float3 radiance = environment_texture.SampleLevel(linear_wrap_sampler, reflect(-frame.V, N), roughness * maxRadianceMipLevel).xyz;

            float3 s = radiance * DFG;

            ambient = (F90 - F) * d + s;
        }

        bool diffuseEnabled = DiffuseEnabled && (iDiffuse != -1);
        if (diffuseEnabled)
        {
            diffuse = base.xyz * (1.0 - metallic) * Fd_Lambert();
        }

        bool specEnabled = SpecEnabled;
        if (specEnabled)
        {
            float3 F90 = float3(1, 1, 1);
            float F0d = 0.04;
            float3 F0 = lerp(F0d.xxx, base.xyz, metallic);

            float3 F = F0;
            if (FresnelEnabled)
            {
                F = F_Schlick(F0, F90, LoH);
            }

            float a = roughness * roughness;

            float D = 1.0f;
            if (MicrofacetEnabled)
            {
                D = D_Beckmann(NoH, a);
            }

            float G = 1.0f;
            if (MaskingEnabled)
            {
                G = G_Smith_Beckmann(NoV, NoL, a);
            }

            diffuse = (F90 - F) * diffuse;
            specular = ((D * G * F) / max((4.0 * NoV * NoL), 0.00001f)) * PI;
        }

        float Sf = 1.0f;
        if (ShadowEnabled)
        {
            Sf = GetShadow(frame);
        }

        float illuminance = NoL * frame.Lx;
        float3 unlit = ambient * max(0.1f, Sf);
        float3 lit = unlit + (diffuse + specular) * frame.Lc * illuminance;
        output.xyz = lerp(unlit, lit, Sf);
    }

    return output;
}

float4 Shade_GGX(ShadingFrame frame)
{
    float4 output = float4(1, 0, 1, 1); // magenta

    int iMaterial = frame.iMaterial;
    if (iMaterial != -1)
    {
        float4 base = float4(0, 0, 0, 0);
        float3 ambient = float3(0, 0, 0);
        float3 diffuse = float3(0, 0, 0);
        float3 specular = float3(0, 0, 0);
        float3 N = frame.N;
        float metallic = 0;
        float roughness = 0;

		int iAlpha = materials[iMaterial].iAlpha;
		if (iAlpha != -1)
		{
			float alpha = textures[iAlpha].Sample(linear_wrap_sampler, frame.UV).r;
			clip(alpha - 0.001f);
		}

        int iDiffuse = materials[iMaterial].iDiffuse;
        if (iDiffuse != -1)
        {
            base = textures[iDiffuse].Sample(aniso_wrap_sampler, frame.UV);
            clip(base.a - 0.1f);
            base.xyz = Gamma_to_Linear(base.xyz);
        }

        int iBump = materials[iMaterial].iBump;
        int iNormal = materials[iMaterial].iNormal;
        bool bumpEnabled = BumpEnabled;
        if (bumpEnabled)
        {
            N = GetNormal(frame, iBump, iNormal, BumpMode);
        }

        int iMetal = materials[iMaterial].iMetal;
        if (iMetal != -1)
        {
            metallic = textures[iMetal].Sample(linear_wrap_sampler, frame.UV).x;
        }

        int iRough = materials[iMaterial].iRough;
        if (iRough != -1)
        {
            roughness = textures[iRough].Sample(linear_wrap_sampler, frame.UV).x;
        }

        float NoV = saturate(dot(N, frame.V));// abs(dot(N, frame.V)) + 1e-5;
        float NoL = saturate(dot(N, frame.L));
        float NoH = saturate(dot(N, frame.H));
        float VoH = saturate(dot(frame.V, frame.H));
        float LoH = saturate(dot(frame.L, frame.H));

        // Unlit
        if (AmbientEnabled)
        {
            // Indirect diffuse;
            const float environmentMipLevels = 6.0;
            const float irradianceMipLevel = environmentMipLevels - 1.0;
            const float maxRadianceMipLevel = irradianceMipLevel - 1.0;

            float3 irradiance = environment_texture.SampleLevel(linear_wrap_sampler, N, irradianceMipLevel).xyz;
            float3 d = (base.xyz * irradiance);

            // Indirect spec
            float roughE = roughness * roughness;
            float F0d = 0.04;

            float3 F90 = float3(1, 1, 1);
            float3 F0 = lerp(F0d.xxx, base.xyz, metallic);
            float3 F = F_Schlick(F0, F90, NoV, roughE);

            float2 brdf = environment_brdf_texture.Sample(linear_wrap_sampler, float2(NoV, roughness)).xy;
            float3 DFG = F0 * brdf.x + brdf.y;

            float3 radiance = environment_texture.SampleLevel(linear_wrap_sampler, reflect(-frame.V, N), roughness * maxRadianceMipLevel).xyz;
            
            float3 s = radiance * DFG;
            
            ambient = (F90 - F) * d + s;
        }

        bool diffuseEnabled = DiffuseEnabled && (iDiffuse != -1);
        if (diffuseEnabled)
        {
            diffuse = base.xyz * (1.0 - metallic) * Fd_Lambert();
        }

        bool specEnabled = SpecEnabled;
        if (specEnabled)
        {
            float3 F90 = float3(1, 1, 1);
            float F0d = 0.04;
            float3 F0 = lerp(F0d.xxx, base.xyz, metallic);

            float3 F = F0;
            if (FresnelEnabled)
            {
                F = F_Schlick(F0, F90, LoH);
            }
 
            float a = roughness * roughness;

            float D = 1.0f;
            if (MicrofacetEnabled)
            {
                D = D_GGX(NoH, a);
            }

            float G = 1.0f;
            if (MaskingEnabled)
            {
                G = G_Smith_GGX(NoV, NoL, a);
            }

            diffuse = (F90 - F) * diffuse;
            specular = (D * G) * F * PI;
        }

        float Sf = 1.0f;
        if (ShadowEnabled)
        {
            Sf = GetShadow(frame);
        }

        float illuminance = NoL * frame.Lx;
        float3 unlit = ambient * max(0.1f, Sf);
        float3 lit = unlit + (diffuse + specular) * frame.Lc * illuminance;
        output.xyz = lerp(unlit, lit, Sf);
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
    frame.Lx = Intensity.w;
    frame.iMaterial = input.mat;

    float4 output;
    if (LightingMode == 0)
    {
        output = Shade_Default(frame);
    }
    else if (LightingMode == 1)
    {
        output = Shade_Blinn_Phong(frame);
    }
    else if (LightingMode == 2)
    {
        output = Shade_Beckmann(frame);
    }
    else
    {
        output = Shade_GGX(frame);
    }

    output.xyz *= Exposure;
    output.xyz = ACES_Film(output.xyz);
    output.xyz = Linear_to_Gamma(output.xyz);

    return output;
}