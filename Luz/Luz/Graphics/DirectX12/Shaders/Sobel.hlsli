float3 SobelFilter(Texture2D height_texture, SamplerState height_sampler, float2 uv, float scale)
{
    /*
        Pixel Coords:
        0,0 | 1,0 | 2,0
        ----+-----+----
        0,1 | 1,1 | 2,1
        ----+-----+----
        0,2 | 1,2 | 2,2
    */

    uint width, height, mips;
    height_texture.GetDimensions(0, width, height, mips);

    float dx = 1.0f / (float)width;
    float dy = 1.0f / (float)height;

    const float2 offsets[9] = 
    {
        float2(-dx, -dy), float2(0.0f, -dy), float2(+dx, -dy),
        float2(-dx,0.0f), float2(0.0f,0.0f), float2(+dx,0.0f),
        float2(-dx, +dy), float2(0.0f, +dy), float2(+dx, +dy)
    };

    float h00 = height_texture.Sample(height_sampler, uv + offsets[0]).r;
    float h10 = height_texture.Sample(height_sampler, uv + offsets[1]).r;
    float h20 = height_texture.Sample(height_sampler, uv + offsets[2]).r;
    float h01 = height_texture.Sample(height_sampler, uv + offsets[3]).r;
    float h21 = height_texture.Sample(height_sampler, uv + offsets[5]).r;
    float h02 = height_texture.Sample(height_sampler, uv + offsets[6]).r;
    float h12 = height_texture.Sample(height_sampler, uv + offsets[7]).r;
    float h22 = height_texture.Sample(height_sampler, uv + offsets[8]).r;

    //		+1  0 -1		     +1 +2 +1		
    // Gx = +2  0 -2		Gy =  0  0  0
    //		+1  0 -1             -1 -2 -1

    // Gx = h00*1 + h10*0 + h20*-1 + h01*2 +h11*0 + h21*-2 + h02*1 + h12*0 + h22*-1
    //	  = h00 - h20 + 2*h01 - 2*h21 + h02 - h22

    // Gy = h00*1 + h10*2 + h20*1 + h01*0 + h11*0 + h21*0 + h02*-1 + h12*-2 + h22*-1
    // 	  = h00 + 2*h10 + h20 - h02 - 2*h12 - h22

    float Gx = h00 - h20 + 2.0f * h01 - 2.0f * h21 + h02 - h22;
    float Gy = h00 + 2.0f * h10 + h20 - h02 - 2.0f * h12 - h22;

    float3 partialX = float3(2.0f, 0.0f, Gx * 2.0f);
    float3 partialY = float3(0.0f, 2.0f, Gy * 2.0f);
    
    float3 normal = cross(partialX, partialY);
    
    return normalize(normal);
}