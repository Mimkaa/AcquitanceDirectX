cbuffer LightCBuf
{
    float3 LightPos;
    float3 light_ambient;
    float3 light_diffuse;
    float attIntensity;
    float constant_attenuation;
    float linear_attenuation;
    float quadratic_attenuation;
};


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

float3 MapNormalViewSpace(const float3 tan, const float3 btan, float3 normalView, const float2 tec, Texture2D nmap, SamplerState splr)
{
    float3x3 TBNmat = float3x3(normalize(tan), normalize(btan), normalize(normalView));
    float3 normalSample = norm.Sample(splr, tec).xyz;
    const float3 tanNormal = normalSample * 2.0f - 1.0f;
        
    return normalize(mul(tanNormal, TBNmat));
     
}

float Attenuation(uniform float constant_attenuation, uniform float linear_attenuation, uniform float quadratic_attenuation, const in float dist)
{
    return 1/(constant_attenuation + linear_attenuation * dist + quadratic_attenuation * pow(dist, 2));
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
    return attenuation * specularIntensity * SpecularColor * pow(max(0.0f, dot(normalize(-r), normalize(viewCamToFrag))), specularPower);
}

float4 main(float3 ViewPos : Position, float3 normalView : Normal, float2 tec : Texcoord, float3 tan : Tangent, float3 btan : Bytangent) : SV_Target
{
    float3 normal = normalize(normalView);
    if (normalsMappingOn == 1)
    {
        normal = MapNormalViewSpace(tan, btan, normalView, tec, norm, smpl);

    }
    
    const float3 v_to_l = LightPos - ViewPos;
    const float dist = length(v_to_l);
    const float3 LightDir = v_to_l / dist;
    const float attenuation = Attenuation(constant_attenuation, linear_attenuation, quadratic_attenuation, dist);

    const float3 d = Diffuse(light_diffuse, attIntensity, attenuation, normal, LightDir);
    
    
    // specular highlight
    const float3 w = normal * dot(v_to_l, normal);
    // opposite direction of the reflection
    const float3 r = w * 2.0f - v_to_l;
    
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
 
    const float3 specular = Speculate(attenuation, 1.0f, specularReflectionColor, normal, v_to_l, ViewPos, specularPower);
    

    return float4(saturate((d + light_ambient) * tex.Sample(smpl, tec).rgb + specular * specularReflectionColor), 1.0f);
}