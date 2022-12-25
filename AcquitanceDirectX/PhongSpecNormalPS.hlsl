#include "PointLight.hlsl"
#include "ShaderOps.hlsl"
#include "LightVectorData.hlsl"

cbuffer MaterialCBuf
{
    bool EnableSpecular;
    float3 SpecularColor;
    float specularWeight;
    bool hasAlphaGloass;
    float specularPowerConst;
    bool normalsMappingOn;
    
};

cbuffer CBufMat
{
    matrix modelView;
    matrix modelViewProj;
};

Texture2D tex;
Texture2D spec;
Texture2D norm;
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

    const float3 d = Diffuse(light_diffuse, attIntensity, attenuation, normal, lv.dirToL);
    
    
    // specular highlight
    const float3 w = normal * dot(lv.vToL, normal);
    // opposite direction of the reflection
    const float3 r = w * 2.0f - lv.vToL;
    
    //specular intensity between view vector an the reflection

    float  specularPower = specularPowerConst;
    float3 specularReflectionColor = SpecularColor * specularWeight;
    if (EnableSpecular)
    {
        const float4 specularSample = spec.Sample(smpl, tec);
        specularReflectionColor = specularSample.rgb * specularWeight;
        if (hasAlphaGloass == 1)
        {
            specularPower = pow(2.0f, specularSample.a * 13.0f);
        }
       
    }
 
    const float3 specular = Speculate(attenuation, 1.0f, specularReflectionColor, normal, lv.vToL, ViewPos, specularPower);
    
    // sample texture
    float4 difSam = tex.Sample(smpl, tec);
    return float4(saturate((d + light_ambient) * difSam.rgb + specular * specularReflectionColor), difSam.a);
}