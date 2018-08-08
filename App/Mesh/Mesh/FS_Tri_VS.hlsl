struct VS_Output
{
    float4 Pos : SV_POSITION;
    float2 UV : TEXCOORD0;
};


VS_Output main(uint id : SV_VERTEXID)
{
    VS_Output output;
    output.Pos.x = (float)(id / 2) * 4.0 - 1.0;
    output.Pos.y = (float)(id % 2) * 4.0 - 1.0;
    output.Pos.z = 0.0;
    output.Pos.w = 1.0;

    output.UV.x = (float)(id / 2) * 2.0;
    output.UV.y = 1.0 - (float)(id % 2) * 2.0;

    return output;
}