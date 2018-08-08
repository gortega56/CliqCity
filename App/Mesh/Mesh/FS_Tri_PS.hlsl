struct PS_Input
{
    float4 Pos : SV_POSITION;
    float2 UV : TEXCOORD0;
};

Texture2D Tex : register(t1);
SamplerState samp : register(s0);

float4 main(PS_Input input) : SV_TARGET
{
    return Tex.Sample(samp, input.UV);
}