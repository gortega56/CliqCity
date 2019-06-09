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
    return pow(max(float3(0, 0, 0), color), 1.0f / 2.2f);
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

// LoH should be clamped to zero
float F_Schlick(float F0, float F90, float LoH)
{
    return F0 + (F90 - F0) * pow(1.0f - LoH, 5.0f);
}

// LoH should be clamped to zero
float3 F_Schlick(float3 F0, float3 F90, float LoH)
{
    return F0 + (F90 - F0) * pow(1.0f - LoH, 5.0f);
}

float F_Schlick(float F0, float F90, float NoV, float roughness)
{
    return F0 + (max(F90 - roughness, F0) - F0) * pow(1.0f - NoV, 5.0f);
}

float3 F_Schlick(float3 F0, float3 F90, float NoV, float roughness)
{
    return F0 + (max(F90 - roughness.xxx, F0) - F0) * pow(1.0f - NoV, 5.0f);
}

float Fd_Lambert()
{
    return 1.0f / PI;
}

float Fd_Burley(float NoV, float NoL, float LoH, float roughness) 
{
    float F90 = 0.5 + 2.0 * roughness * LoH * LoH;
    float lightScatter = F_Schlick(1.0, F90, NoL);
    float viewScatter = F_Schlick(1.0, F90, NoV);
    return lightScatter * viewScatter * (1.0 / PI);
}

float D_Blinn_Phong(float NoH, float roughness)
{
    return ((roughness + 2.0) / 2.0 * PI) * pow(NoH, roughness);
}

// NoH should be clamped to zero
float D_Beckmann(float NoH, float roughness)
{
    float a = roughness * roughness;
    float k = NoH * NoH;
    return (1.0f / (PI * a * k * k)) * exp((k - 1.0) / (a * k));
}

// NoH should be clamped to zero
float D_GGX(float NoH, float roughness)
{
    //http://graphicrants.blogspot.com/2013/08/specular-brdf-reference.html
    //float a = NoH * roughness;
    //float k = roughness / (1.0 - NoH * NoH + a * a);
    //return k * k * (1.0 / PI);

    //http://www.realtimerendering.com/#brdf
    // Includes an extra factor of NoH in the numerator?
    //return NoH * k * k * (1.0 / PI);

    //https://www.shadertoy.com/view/4sSfzK
    //float a = roughness * roughness;
    //float k = (NoH * a - NoH) * NoH + 1.0;
    //return a / (k * k * PI);

    float a = roughness * roughness;
    float k = (NoH * NoH) * (a - 1.0) + 1.0;
    return a / (k * k * PI);
}

float G_Implicit(float NoV, float NoL)
{
    return 1.0f / 4.0f;
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

    //http://www.realtimerendering.com/#brdf
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
    return (G1_Smith_Beckmann(NoV, roughness) * G1_Smith_Beckmann(NoL, roughness)) / max((4.0 * NoV * NoL), 0.00001f);
}

// NoV clamped to zero
// NoL clamped to zero
float G_Smith_GGX(float NoV, float NoL, float roughness)
{
    //http://www.realtimerendering.com/#brdf
    //https://google.github.io/filament/Filament.md.html#toc1
    float a = roughness * roughness;
    float v = NoL * sqrt(NoV * NoV * (1.0f - a) + a);
    float l = NoV * sqrt(NoL * NoL * (1.0f - a) + a);
    return 0.5f / max(v + l, 0.00001f);

    // Karis http://graphicrants.blogspot.com/2013/08/specular-brdf-reference.html
    //float a = roughness * roughness;
    //float v = NoV + sqrt((NoV - NoV * a) * NoV + a);
    //float l = NoL + sqrt((NoL - NoL * a) * NoL + a);
    //return rcp(v * l);
}

float G_Schlick_GGX(float NoV, float NoL, float roughness)
{
    float k = (roughness * roughness) * 0.5f;
    float v = NoV / (NoV * (1.0 - k) + k);
    float l = NoL / (NoL * (1.0 - k) + k);
    return v * l;
}

// https://www.unrealengine.com/en-US/blog/physically-based-shading-on-mobile
float3 DFG_Env_Karis(float3 F0, float NoV, float roughness)
{
    const float4 c0 = float4(-1, -0.0275, -0.572, 0.022);
    const float4 c1 = float4(1, 0.0425, 1.04, -0.04);
    float4 r = roughness * c0 + c1;
    float a004 = min(r.x * r.x, exp2(-9.28 * NoV)) * r.x + r.y;
    float2 AB = float2(-1.04, 1.04) * a004 + r.zw;
    return F0 * AB.x + AB.y;
}

//https://knarkowicz.wordpress.com/2014/12/27/analytical-dfg-term-for-ibl/
float3 DFG_Env_Knarco(float3 F0, float NoV, float roughness)
{
    float x = 1.0 - roughness;
    float y = NoV;

    float b1 = -0.1688;
    float b2 = 1.895;
    float b3 = 0.9903;
    float b4 = -4.853;
    float b5 = 8.404;
    float b6 = -5.069;
    float bias = saturate(min(b1 * x + b2 * x * x, b3 + b4 * y + b5 * y * y + b6 * y * y * y));

    float d0 = 0.6045;
    float d1 = 1.699;
    float d2 = -0.5228;
    float d3 = -3.603;
    float d4 = 1.404;
    float d5 = 0.1939;
    float d6 = 2.661;
    float delta = saturate(d0 + d1 * x + d2 * y + d3 * x * x + d4 * x * y + d5 * y * y + d6 * x * x * x);
    float scale = delta - bias;

    bias *= saturate(50.0 * F0.y);
    return F0 * scale + bias;
}

//https://learnopengl.com/PBR/IBL/Diffuse-irradiance
float3 Environment_Irradiance(TextureCube cube_texture, SamplerState cube_sampler, float3 normal)
{
    float3 irradiance = float3(0, 0, 0);

    float3 up = float3(0, 1, 0);
    float3 right = cross(up, normal);
    up = cross(normal, right);

    float dt = 0.025f;
    float nSamples = 0.0;

    for (float phi = 0.0; phi < 2.0 * PI; phi += dt)
    {
        float cos_phi = cos(phi);
        float sin_phi = sin(phi);

        for (float theta = 0.0; theta < 0.5 * PI; theta += dt)
        {
            float cos_theta = cos(theta);
            float sin_theta = sin(theta);

            float3 tangent = float3(sin_theta * cos_phi, sin_theta * sin_phi, cos_theta);
            float3 uvw = tangent.x * right + tangent.y * up + tangent.z * normal;
            irradiance += cube_texture.Sample(cube_sampler, uvw).rgb * cos_theta * sin_theta;

            nSamples++;
        }
    }

    return irradiance * PI * (1.0 / nSamples);
}

float radical_inverse_vdc(uint bits)
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10;
}

float2 hammersley(uint i, uint n)
{
    return float2(float(i) / float(n), radical_inverse_vdc(i));
}

float3 importance_sample_ggx(float2 sequence, float3 N, float roughness)
{
    float a = roughness * roughness;
    float phi = 2.0 * PI * sequence.x;
    float cos_theta = sqrt((1.0 - sequence.y) / (1.0 + (a * a - 1.0) * sequence.y));
    float sin_theta = sqrt(1.0 - cos_theta * cos_theta);

    // spherical -> cartesian
    float3 H;
    H.x = cos(phi) * sin_theta;
    H.y = sin(phi) * sin_theta;
    H.z = cos_theta;

    // tangent space -> world space
    float3 normal = abs(N.z) < 0.999 ? float3(0, 0, 1) : float3(1, 0, 0);
    float3 tangent = normalize(cross(normal, N));
    float3 bitangent = cross(N, tangent);

    return normalize(tangent * H.x + bitangent * H.y + N * H.z);
}

//https://learnopengl.com/PBR/IBL/Specular-IBL
float3 Environment_Radiance(TextureCube cube_texture, SamplerState cube_sampler, float3 normal, float roughness)
{
    float3 N = normal;
    float3 R = N;
    float3 V = R;

    float3 radiance = float3(0, 0, 0);
    float weight = 0.0;

    const uint nSamples = 1024;
    for (uint i = 0; i < nSamples; ++i)
    {
        float2 sequence = hammersley(i, nSamples);
        float3 H = importance_sample_ggx(sequence, N, roughness);
        float3 L = normalize(2.0 * dot(V, H) * H - V);

        float NoL = saturate(dot(N, L));
        if (NoL > 0.0)
        {
            radiance += cube_texture.SampleLevel(cube_sampler, L, 0).rgb * NoL;
            weight += NoL;
        }
    }

    return radiance / weight;
}

float2 environment_brdf(float NoV, float roughness)
{
    float3 V;
    V.x = sqrt(1.0 - NoV * NoV);
    V.y = 0.0;
    V.z = NoV;

    float3 N = float3(0, 0, 1);

    float A = 0;
    float B = 0;

    const uint nSamples = 1024;
    for (uint i = 0; i < nSamples; ++i)
    {
        float2 sequence = hammersley(i, nSamples);
        float3 H = importance_sample_ggx(sequence, N, roughness);
        float3 L = normalize(2.0 * dot(V, H) * H - V);

        float NoL = saturate(L.z);
        float NoH = saturate(H.z);
        float VoH = saturate(dot(V, H));

        if (NoL > 0)
        {
            float G = G_Schlick_GGX(NoV, NoL, roughness);
            float G_Vis = G * VoH / (NoH * NoV);
            float Fc = pow(1 - VoH, 5);
            A += (1 - Fc) * G_Vis;
            B += Fc * G_Vis;
        }
    }
        
    return float2(A, B) / float(nSamples);
}