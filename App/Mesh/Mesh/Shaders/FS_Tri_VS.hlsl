struct VS_Output
{
    float4 Pos : SV_POSITION;
    float2 UV : TEXCOORD0;
};


VS_Output main(uint id : SV_VERTEXID)
{
    VS_Output output;
    output.UV = float2((id << 1) & 2, id & 2);
    output.Pos = float4(output.UV * float2(2, -2) + float2(-1, 1), 0, 1);
    return output;
}