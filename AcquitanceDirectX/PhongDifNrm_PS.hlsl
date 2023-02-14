#include "PointLight.hlsl"
#include "ShaderOps.hlsl"
#include "LightVectorData.hlsl"

cbuffer MaterialCBuf : register(b1)
{
    float3 specularColor;
    float specularWeight;
    float specularGloss;
    bool useNormalMap;
    float normalMapWeight;
};

#include "Transformation.hlsl"

Texture2D tex : register(t0);
Texture2D norm: register(t2);
SamplerState smpl : register(s0);;

float4 main(float3 ViewPos : Position, float3 normalView : Normal, float2 tec : Texcoord, float3 tan : Tangent, float3 btan : Bytangent) : SV_Target
{
    normalView = normalize(normalView);
    if (useNormalMap)
    {
        const float3 mappedNormal = MapNormalViewSpace(normalize(tan), normalize(btan), normalize(normalView), tec, norm, smpl);
        normalView = lerp(normalView, mappedNormal, normalMapWeight);
    }
    
    const LightVectorData lv = CalculateLightVectorData(LightPos, ViewPos);
    const float attenuation = Attenuation(constant_attenuation, linear_attenuation, quadratic_attenuation, lv.distToL);


    const float3 d = Diffuse(light_diffuse, attIntensity, attenuation, normalView, lv.dirToL);;

    
//specular intensity between view vector and the reflection
    const float3 specular = Speculate(attenuation, specularWeight, specularColor,
        normalView, lv.vToL, ViewPos,specularGloss);
    

return float4(saturate((d + light_ambient) * tex.Sample(smpl, tec).rgb + specular), 1.0f);

    
}