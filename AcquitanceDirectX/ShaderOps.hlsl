float3 MapNormalViewSpace(const float3 tan, const float3 btan, float3 normalView, const float2 tec, Texture2D nmap, SamplerState splr)
{
    float3x3 TBNmat = float3x3(tan, btan, normalView);
    float3 normalSample = nmap.Sample(splr, tec).xyz;
    const float3 tanNormal = normalSample * 2.0f - 1.0f;
        
    return normalize(mul(tanNormal, TBNmat));
     
}

float Attenuation(uniform float constant_attenuation, uniform float linear_attenuation, uniform float quadratic_attenuation, const in float dist)
{
    return 1 / (constant_attenuation + linear_attenuation * dist + quadratic_attenuation * pow(dist, 2));
}

float3 Diffuse(uniform float3 light_diffuse, uniform float attIntensity, const in float attenuation, const in float3 normal, const in float3 LightDir)
{
    return light_diffuse * attenuation * attIntensity * max(0.0f, dot(normal, LightDir));
}

float3 Speculate(const in float attenuation, const in float specularIntensity, const in float3 SpecularColor, const in float3 normal,
const in float3 v_to_l, const in float3 ViewPos, const in float specularPower
)
{
    // specular highlight
    const float3 w = normal * dot(v_to_l, normal);
    // opposite direction of the reflection
    const float3 r = w * 2.0f - v_to_l;
    // vector from camera to fragment (in view space) renormalization is used here
    const float3 viewCamToFrag = normalize(ViewPos);
    return attenuation * specularIntensity * SpecularColor * pow(dot(normalize(-r), normalize(viewCamToFrag)), specularPower);
}