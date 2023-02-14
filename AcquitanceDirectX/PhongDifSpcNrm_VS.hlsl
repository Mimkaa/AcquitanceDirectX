cbuffer CBuf
{
    matrix modelView;
    matrix modelViewProj;
};

struct VSOut
{
    float3 viewPos : Position;
    float3 viewNormal : Normal;
    float2 te: Texcoord;
    float3 tan: Tangent;
    float3 btan: Bytangent;
    float4 pos : SV_Position;

};

VSOut main(float3 pos : Position, float3 n : Normal, float2 tec : Texcoord, float3 tan : Tangent, float3 btan: Bytangent)
{
    VSOut vso;
    vso.viewPos = (float3) mul(float4(pos, 1.0f), modelView);
    vso.viewNormal = mul(n, (float3x3) modelView);
    vso.pos = mul(float4(pos, 1.0f), modelViewProj);
    vso.te = tec;
    vso.tan = mul(tan, (float3x3) modelView);
    vso.btan = mul(btan, (float3x3) modelView);

    return vso;
}