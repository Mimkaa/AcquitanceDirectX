Texture2D tex;
SamplerState splr;

static const float r = 7;
static const float div = (r * 2 + 1) * (r * 2 + 1);

float4 main(float2 uv : Texcoord) : SV_Target
{
    uint width, height;
    tex.GetDimensions(width, height);
    float dx = 1.0f / width;
    float dy = 1.0f / height;
    

    float accAlpha = 0.0f;
    float3 maxColor = float3(0.0f, 0.0f, 0.0f);
    for (int y = -r; y <= r; y++)
    {
        for (int x = -r; x <= r; x++)
        {
            float4 sample = tex.Sample(splr, uv + float2(x * dx, y * dx)).rgba;
            accAlpha += sample.a;
            maxColor = max(sample.rgb, maxColor);
            
        }

    }
    return float4(maxColor.rgb, accAlpha / div);
}