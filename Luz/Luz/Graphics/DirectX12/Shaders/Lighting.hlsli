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