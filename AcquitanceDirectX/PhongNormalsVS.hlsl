cbuffer CBuf
{
    matrix modelView;
    matrix modelViewProj;
};

struct VSOut
{
    float3 worldPos : Position;
    float2 te: Texcoord;
    float4 pos : SV_Position;

};

VSOut main(float3 pos : Position, float2 tec : Texcoord)
{
    VSOut vso;
    vso.worldPos = (float3) mul(float4(pos, 1.0f), modelView);
    vso.pos = mul(float4(pos, 1.0f), modelViewProj);
    vso.te = tec;
    return vso;
}