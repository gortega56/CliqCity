static const float PI = 3.14159265359f;

// Kd - Diffuse Color
// N - Normal
// L - Light Direction
// Lc - Light Color
// Id - Light Intensity
float3 Blinn_Phong_Diffuse(float3 Kd, float3 N, float3 L, float3 Lc, float Id)
{
    return Kd * saturate(dot(N, L)) * Lc * Id;
}

// Ks - Specular Color
// N - Normal
// H - Half (Light / View Bisector)
// Lc - Light Color
// Es - Specular Exponent
// Is - Light Intensity
float3 Blinn_Phong_Spec(float3 Ks, float3 N, float3 H, float3 Lc, float Es, float Is)
{
    return Ks * pow(saturate(dot(N, H)), Es) * Lc * Is;
}

float Shadow_Factor(Texture2D shadow_texture, SamplerComparisonState shadow_sampler, float2 uv, float depth)
{
    uint width, height, mips;
    shadow_texture.GetDimensions(0, width, height, mips);

    float dx = 1.0f / (float)width;

    float percent_lit = 0.0f;
    const float2 offsets[9] =
    {
        float2(-dx, -dx), float2(0.0f, -dx), float2(dx, -dx),
        float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
        float2(-dx, dx), float2(0.0f, dx), float2(dx, dx)
    };

    [unroll]
    for (int i = 0; i < 9; ++i)
    {
        percent_lit += shadow_texture.SampleCmpLevelZero(shadow_sampler, uv + offsets[i], depth).r;
    }

    return percent_lit / 9.0f;
}

float3 Linear_to_Gamma(float3 color)
{
    return pow(color, 1.0f / 2.2f);
}

float3 Gamma_to_Linear(float3 color)
{
    return pow(color, 2.2f);
}

float3 Linear_to_SRGB(float3 color)
{
    float3 lo = color * 12.92f;
    float3 hi = 1.055f * pow(saturate(color), 1.0f / 2.4f) - 0.055f;
    color.x = (color.x < 0.0031308f) ? lo.x : hi.x;
    color.y = (color.x < 0.0031308f) ? lo.y : hi.y;
    color.z = (color.x < 0.0031308f) ? lo.z : hi.z;
    return color;
}

float3 SRGB_to_Linear(float3 color)
{
    float3 lo = color / 12.92f;
    float3 hi = pow(max((color + 0.055f) / 1.055f,  0.0f), 2.4f);
    color.x = (color.x < 0.04045f) ? lo.x : hi.x;
    color.y = (color.x < 0.04045f) ? lo.y : hi.y;
    color.z = (color.x < 0.04045f) ? lo.z : hi.z;
    return color;
}


float3 Reinhard_Simple(float3 color, float exposure)
{
    return color * exposure / (1.0f + color / exposure);
}

float3 Reinhard_Luma(float3 color)
{
    float luma = dot(color, float3(0.2126f, 0.7152f, 0.0722f));
    float toneMapped = luma / (1.0f + luma);
    return color * toneMapped / luma;
}

float3 ACES_Film(float3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return saturate((x*(a*x + b)) / (x*(c*x + d) + e));
}

// NoV should be clamped to zero
float F_Schlick(float F0, float F90, float NoV)
{
    return F0 + (F90 - F0) * pow(1.0f - NoV, 5.0f);
}

// NoV should be clamped to zero
float3 F_Schlick(float3 F0, float3 F90, float NoV)
{
    return F0 + (F90 - F0) * pow(1.0f - NoV, 5.0f);
}

float Fd_Burley(float NoV, float NoL, float LoH, float roughness) 
{
    float F90 = 0.5 + 2.0 * roughness * LoH * LoH;
    float lightScatter = F_Schlick(1.0, F90, NoL);
    float viewScatter = F_Schlick(1.0, F90, NoV);
    return lightScatter * viewScatter * (1.0 / PI);
}
// NoH should be clamped to zero
float D_Beckmann(float NoH, float roughness)
{
    float rough_sq = roughness * roughness;
    return  (ceil(NoH) / (PI * rough_sq * NoH * NoH * NoH * NoH)) * exp((NoH * NoH - 1.0f) / (rough_sq * NoH * NoH));
}

// NoH should be clamped to zero
float D_Trowbridge_Reitz(float NoH, float roughness)
{
    float a = NoH * roughness;
    float k = roughness / (1.0 - NoH * NoH + a * a);
    return k * k * (1.0 / PI);
}

// NoS clamped to zero
float G1_Smith_Beckmann(float NoS, float roughness)
{
    // Karis http://graphicrants.blogspot.com/2013/08/specular-brdf-reference.html
    const float a = 3.535f;
    const float b = 2.181f;
    const float d = 2.276f;
    const float e = 2.577f;

    float c = NoS / (roughness * sqrt(1.0f - NoS * NoS));
    if (c < 1.6f)
    {
        c = (a * c + b * c * c) / (1.0f + d * c + e * c * c);
    }
    else
    {
        c = 1.0f;
    }

    // Real-Time Rendering 
    //const float a = 1.259f;
    //const float b = 0.396f;
    //const float d = 3.535f;
    //const float e = 2.181f;

    //float c = NoS / (roughness * sqrt(1.0f - NoS * NoS));
    //if (c < 1.6f)
    //{
    //    c = (1.0f - a * c + b * c * c) / (d * c + e * c * c);
    //}
    //else
    //{
    //    c = 0.0f;
    //}

    return c;
}

// NoV clamped to zero
// NoL clamped to zero
float G_Smith_Beckmann(float NoV, float NoL, float roughness)
{
    return G1_Smith_Beckmann(NoV, roughness) * G1_Smith_Beckmann(NoL, roughness);
}

// NoV clamped to zero
// NoL clamped to zero
float G_Smith_Trowbridge_Reitz(float NoV, float NoL, float roughness)
{
    float a = roughness * roughness;
    float ggxv = NoL * sqrt(NoV * NoV * (1.0f - a) + a);
    float ggxl = NoV * sqrt(NoL * NoL * (1.0f - a) + a);
    return 0.5f / (ggxv + ggxl);
}