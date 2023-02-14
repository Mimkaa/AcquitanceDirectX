#include "ShaderOps.hlsl"
#include "LightVectorData.hlsl"

#include "PointLight.hlsl"

cbuffer ObjectCBuf
{
    bool useGlossAlpha;
    float3 specularColor;
    float specularWeight;
    float specularGloss;
};

Texture2D tex;
Texture2D spec;

SamplerState splr;



float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal, float2 tc : Texcoord) : SV_Target
{
    // normalize the mesh normal
    viewNormal = normalize(viewNormal);
	// fragment to light vector data
    const LightVectorData lv = CalculateLightVectorData(LightPos, viewFragPos);
    // specular parameters
    float specularPower = specularGloss;
    const float4 specularSample = spec.Sample(splr, tc);
    const float3 specularReflectionColor = specularSample.rgb * specularWeight;
    if (useGlossAlpha)
    {
        specularPower = pow(2.0f, specularSample.a * 13.0f);
    }
	// attenuation
    const float att = Attenuation(constant_attenuation, linear_attenuation, quadratic_attenuation, lv.distToL);
	// diffuse light
    const float3 diffuse = Diffuse(light_diffuse, attIntensity, att, lv.dirToL, viewNormal);
    // specular reflected
    const float3 specularReflected = Speculate(att, 1.0f, specularReflectionColor, viewNormal, lv.vToL, viewFragPos, specularPower);
	// final color = attenuate diffuse & ambient by diffuse texture color and add specular reflected
    return float4(saturate((diffuse + light_ambient) * tex.Sample(splr, tc).rgb + specularReflected), 1.0f);
}