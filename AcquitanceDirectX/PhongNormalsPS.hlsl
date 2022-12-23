#include "PointLight.hlsl"
#include "ShaderOps.hlsl"
#include "LightVectorData.hlsl"

cbuffer MaterialCBuf
{
    float specularIntensity;
    float specularPower;
    bool normalsMappingOn;
    float padding[1];
};

#include "Transformation.hlsl"

Texture2D tex;
Texture2D norm: register(t2);
SamplerState smpl;

float4 main(float3 ViewPos : Position, float3 normalView : Normal, float2 tec : Texcoord, float3 tan : Tangent, float3 btan : Bytangent) : SV_Target
{
    float3 normal = normalize(normalView);
    if (normalsMappingOn == 1)
    {
        normal = MapNormalViewSpace(normalize(tan), normalize(btan), normalize(normalView), tec, norm, smpl);
        
    }
    
    const LightVectorData lv = CalculateLightVectorData(LightPos, ViewPos);
    const float attenuation = Attenuation(constant_attenuation, linear_attenuation, quadratic_attenuation, lv.distToL);


    const float3 d = Diffuse(light_diffuse, attIntensity, attenuation, normal, lv.dirToL);;

    
//specular intensity between view vector an the reflection
 const float3 specular = Speculate(attenuation, 1.0f, specularIntensity.rrr, normal, lv.vToL, ViewPos, specularPower);
    

return float4(saturate((d + light_ambient) * tex.Sample(smpl, tec).rgb + specular), 1.0f);

    
}