Texture2D tex;
SamplerState splr;
 
cbuffer Kernel
{
    uint Taps;
    float coefficients[15];
};

cbuffer Constrol
{
    bool horizontal;
};



float4 main(float2 uv : Texcoord) : SV_Target
{
    uint width, height;
    tex.GetDimensions(width, height);
    float dx, dy;
    const int r = Taps / 2;
    
    if (horizontal)
    {
        dx = 1.0f / width;
        dy = 0.0f;
    }
    else
    {
        dx = 0.0f;
        dy = 1.0f/height;
    }
    
  
    //float3 maxColor = float3(0.0f, 0.0f, 0.0f);
    float4 accumulation = float4(0.0f, 0.0f, 0.0f, 0.0f);
    for (int i = -r; i <=r; i++)
    {
        float2 coord = uv + float2(i * dx, i * dy);
        float4 sample = tex.Sample(splr, coord).rgba;
        sample *= coefficients[i + r];
        //maxColor = max(sample.rgb, maxColor);
        accumulation += sample;
    }
    return accumulation;
}