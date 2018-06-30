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
    return Ks * pow(saturate(dot(N, H)), Es) * Lc * Is
}